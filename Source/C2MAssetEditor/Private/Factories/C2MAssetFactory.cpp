#include "C2MAssetFactory.h"
#include "Containers/UnrealString.h"
#include "C2MAsset.h"
#include "C2MAssetEditorStyle.h"
#include "C2MapHeader.h"
#include "C2Mesh.h"
#include "C2MImportUI.h"
#include "ToolMenus.h"
#include "Engine.h"
#include "Misc/FileHelper.h"
#include "AssetToolsModule.h"
#include "C2Material.h"
#include "C2MStaticMesh.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Misc/ScopedSlowTask.h"
#include "Commandlets/ImportAssetsCommandlet.h"
#include "FileHelpers.h"

/* UTextAssetFactory structors
 *****************************************************************************/

UC2MAssetFactory::UC2MAssetFactory( const FObjectInitializer& ObjectInitializer )
	: Super(ObjectInitializer)
{
	Formats.Add(TEXT("c2m;C2M File"));
	SupportedClass = UC2MAsset::StaticClass();
	bCreateNew = false;
	bEditorImport = true;
}


/* UFactory overrides
 *****************************************************************************/


UObject* UC2MAssetFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	FScopedSlowTask SlowTask(5, NSLOCTEXT("C2MFactory", "BeginReadC2MFile", "Opening C2M file."), true);
	if (Warn->GetScopeStack().Num() == 0)
	{
		// We only display this slow task if there is no parent slowtask, because otherwise it is redundant and doesn't display any relevant information on the progress.
		// It is primarly used to regroup all the smaller import sub-tasks for a smoother progression.
		SlowTask.MakeDialog(true);
	}
	SlowTask.EnterProgressFrame(0);
	// Create our base C2M Asset
	UC2MAsset* C2MAsset = nullptr;
	// Load C2M File
	TArray64<uint8> FileDataOld;
	if (FFileHelper::LoadFileToArray(FileDataOld, *Filename))
	{
		C2MAsset = NewObject<UC2MAsset>(InParent, InClass, InName, Flags);
		C2MAsset->Path = *Filename;
		auto FileData = FileDataOld.GetData();
		FLargeMemoryReader Reader(FileData, FileDataOld.Num());
		// Parse C2M File Header
		C2MAsset->Header = NewObject<UC2MapHeader>(C2MAsset);
		C2MAsset->Header->ParseHeader(Reader);
		BaseDestPath += C2MAsset->Header->GameVersion + "/";
		/* Import Options Dialog
		 * Let user decide what and how to import our C2M file */
		C2MAsset->ImportOptions = NewObject<UC2MImportOptions>();
		C2MAsset->ImportOptions->ParseGameSpecificOptions(C2MAsset->Header);
		UC2MImportUI::CreateC2MImportWindow(C2MAsset->Header, C2MAsset->ImportOptions);

		if(!C2MAsset->ImportOptions->bShouldImport)
		{
			bOutOperationCanceled = true;
			C2MAsset->ConditionalBeginDestroy();
			return NULL;
		}
		
		TArray<C2Material*> MapMaterials;
		// Do we import materials?
		if(C2MAsset->ImportOptions->bImportMaterials)
		{
			// Move to materials offset
			Reader.Seek(C2MAsset->Header->MaterialsOffset);
			// Move to next SlowTask frame
			SlowTask.EnterProgressFrame(1, FText::FromString("Importing Materials & Textures"));
			
			int MaterialCount = C2MAsset->Header->MaterialsCount;
			MapMaterials.Reserve(MaterialCount);
			// Setup Disk images folder
			FString DiskTexturesPath;
			FString SRight;
			Filename.Split(TEXT("exported_maps"), &DiskTexturesPath, &SRight);
			DiskTexturesPath += "exported_images/" + C2MAsset->Header->GameVersion + "/";
			FString UnrealTexturesPath = BaseDestPath + "Textures/";
			
			// Create our materials slow task
			FFormatNamedArguments Args;
			Args.Add(TEXT("MateriaIndex"), 1);
			Args.Add(TEXT("ArrayLength"), MaterialCount);
			Args.Add(TEXT("MaterialName"), FText::FromString(""));
			FText MaterialSlowTaskText = NSLOCTEXT("C2MFactory", "C2MCreatingMaterial", "{MateriaIndex} of {ArrayLength} - {MaterialName}");
			FScopedSlowTask MaterialSlowTask(MaterialCount, FText::Format(MaterialSlowTaskText, Args));
			MaterialSlowTask.MakeDialog();
			for (int i =0; i < MaterialCount; i++)
			{
				C2Material* CoDMaterial = new C2Material();
				CoDMaterial->ParseMaterial(Reader);
				MapMaterials.Push(CoDMaterial);
				// Update slow task
				Args[TEXT("MateriaIndex")] = i + 1;
				Args[TEXT("MaterialName")] = FText::FromString(CoDMaterial->Header->MaterialName);
				MaterialSlowTask.EnterProgressFrame(1, FText::Format(MaterialSlowTaskText, Args));
				for (auto& Texture : CoDMaterial->Textures)
				{
					//UObject* TextureAsset = LoadObject<UObject>(nullptr, *(UnrealTexturesPath + C2MTexture::NoIllegalSigns(Texture.TextureName)));
					FString ImageFilePath = DiskTexturesPath + Texture.TextureName + ".TGA";
					if (!FPackageName::DoesPackageExist(FPaths::Combine(UnrealTexturesPath, C2MTexture::NoIllegalSigns(Texture.TextureName))) &&FPaths::FileExists(ImageFilePath))
						ImportTexture(ImageFilePath, UnrealTexturesPath);
					else
						UE_LOG(LogTemp, Log, TEXT("NICE"));
				}
			}
		}
		// Import Map Geometry
		SlowTask.EnterProgressFrame(2, FText::FromString("Importing Map Geometry"));
		Reader.Seek(C2MAsset->Header->ObjectsOffset);
		C2Mesh* MapGeometry = new C2Mesh();
		MapGeometry->ParseMesh(Reader);
		C2MStaticMesh::CreateStaticMesh(C2MAsset->Header, true, MapGeometry, C2MAsset->ImportOptions, MapMaterials);
		/* Do we import XModels?
		* We don't need to seek our objects offset, since xmodels are right after mapGeometry model */
		if(C2MAsset->ImportOptions->bImportModels)
		{
			SlowTask.EnterProgressFrame(2, FText::FromString("Importing Models"));
			int ModelCount = C2MAsset->Header->ObjectsCount - 1;
			// Create our models slow task
			FFormatNamedArguments Args;
			Args.Add(TEXT("ModelIndex"), 1);
			Args.Add(TEXT("ArrayLength"), ModelCount);
			Args.Add(TEXT("ModelName"), FText::FromString(""));
			FText ModelSlowTaskText = NSLOCTEXT("C2MFactory", "C2MCreatingModel", "{ModelIndex} of {ArrayLength} - {ModelName}");
			FScopedSlowTask ModelSlowTask(ModelCount, FText::Format(ModelSlowTaskText, Args));
			ModelSlowTask.MakeDialog();
			// Start importing models
			for (int i = 0; i < ModelCount; i++)
			{
				// Read Mesh
				C2Mesh* MapModel = new C2Mesh();
				MapModel->ParseMesh(Reader);
				if (MapModel->Header->MeshName.IsEmpty())
					continue;
				// Update slow task
				Args[TEXT("ModelIndex")] = i + 1;
				Args[TEXT("ModelName")] = FText::FromString(MapModel->Header->MeshName);
				ModelSlowTask.EnterProgressFrame(1, FText::Format(ModelSlowTaskText, Args));
				// Create Static Mesh
				FString pathPackage = FPaths::Combine(TEXT("/Game"),
										  TEXT("C2M"),
										  C2MAsset->Header->GameVersion,
										  TEXT("Models"),
										  MapModel->Header->MeshName);
				if(!FPackageName::DoesPackageExist(pathPackage))
					C2MStaticMesh::CreateStaticMesh(C2MAsset->Header, false, MapModel, C2MAsset->ImportOptions, MapMaterials);
			}
		}

		C2MAsset->CreateOptions = NewObject<UC2MCreateOptions>(C2MAsset);
		C2MAsset->CreateOptions->MapPath = C2MAsset->Path;
		C2MAsset->CreateOptions->MapName = C2MAsset->Header->MapName;
		C2MAsset->CreateOptions->GameVersion = C2MAsset->Header->GameVersion;
		C2MAsset->CreateOptions->MaterialCount = FString::FromInt(C2MAsset->Header->MaterialsCount);
		C2MAsset->CreateOptions->ModelCount = FString::FromInt(C2MAsset->Header->ObjectsCount);
		C2MAsset->CreateOptions->InstanceCount = FString::FromInt(C2MAsset->Header->InstancesCount);
		C2MAsset->CreateOptions->LightCount = FString::FromInt(C2MAsset->Header->LightsCount);
		// Import complete.
		FileDataOld.Empty();
	}

	bOutOperationCanceled = false;

	return C2MAsset;
}

void UC2MAssetFactory::ImportTexture(FString FilePath, FString DestinationPath)
{
	UAutomatedAssetImportData* importData = NewObject<UAutomatedAssetImportData>();
	importData->bReplaceExisting = true;
	importData->DestinationPath = DestinationPath;
	importData->Filenames = TArray<FString>{ FilePath };
	FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
	auto importedTexture = Cast<UTexture2D>(AssetToolsModule.Get().ImportAssetsAutomated(importData)[0]);
	FString PackageFileName = FPackageName::LongPackageNameToFilename(importedTexture->GetPathName(), FPackageName::GetAssetPackageExtension());
	auto Package = importedTexture->GetPackage();
	Package->FullyLoad();
	Package->Modify();
	if(FilePath.EndsWith("_nog.TGA"))
	{
		importedTexture->CompressionSettings = TC_Default;
		importedTexture->SRGB = false;
	}
	importedTexture->MarkPackageDirty();
	// Importing hundreds/thousands of textures without saving will probably cause UE to crash due to lack of memory, so we save after every import.
	//UPackage::SavePackage(importedAssets[0]->GetPackage(), nullptr, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *(importData->DestinationPath + FPackageName::GetAssetPackageExtension()));
	UEditorLoadingAndSavingUtils::SavePackages(TArray<UPackage*>{importedTexture->GetPackage()}, false);
	//UPackage::SavePackage(Package, importedTexture, RF_Public | RF_Standalone, *PackageFileName, GError, nullptr, false, true, SAVE_NoError);
}

