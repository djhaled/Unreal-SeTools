#include "Factories/C2ModelAssetFactory.h"
#include "Containers/UnrealString.h"
#include "Structures/C2Mesh.h"
#include "Misc/FileHelper.h"
#include "AssetToolsModule.h"
#include "Editor.h"
#include "Structures/C2Material.h"
#include "AssetTool/C2MStaticMesh.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Misc/ScopedSlowTask.h"
#include "Commandlets/ImportAssetsCommandlet.h"
#include "FileHelpers.h"
#include "Subsystems/AssetEditorSubsystem.h"

/* UTextAssetFactory structors
 *****************************************************************************/

UC2ModelAssetFactory::UC2ModelAssetFactory( const FObjectInitializer& ObjectInitializer )
	: Super(ObjectInitializer)
{
	Formats.Add(TEXT("semodel;SeModel File"));
	SupportedClass = UStaticMesh::StaticClass();
	bCreateNew = false;
	bEditorImport = true;
}


/* UFactory overrides
 *****************************************************************************/


UObject* UC2ModelAssetFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	FScopedSlowTask SlowTask(5, NSLOCTEXT("C2MFactory", "BeginReadC2MFile", "Opening C2Model file."), true);
	if (Warn->GetScopeStack().Num() == 0)
	{
		// We only display this slow task if there is no parent slowtask, because otherwise it is redundant and doesn't display any relevant information on the progress.
		// It is primarly used to regroup all the smaller import sub-tasks for a smoother progression.
		SlowTask.MakeDialog(true);
	}
	SlowTask.EnterProgressFrame(0);
	// Create our base C2M Asset
	UObject* MeshCreated = nullptr;
	// Load C2M File
	TArray64<uint8> FileDataOld;
	if (FFileHelper::LoadFileToArray(FileDataOld, *Filename))
	{
		FLargeMemoryReader Reader(FileDataOld.GetData(), FileDataOld.Num());
		C2Mesh* Mesh = new C2Mesh();
		Mesh->ParseMesh(Reader);

		Mesh->Header->MeshName =  FPaths::GetBaseFilename(Filename);
		FString wepathPackage = FPaths::Combine(TEXT("/Game"), Mesh->Header->GameName, TEXT("Models"));
		TArray<C2Material*> C2Materials;
		//
		FString DiskTexturesPath = FPaths::GetPath(FPaths::GetPath(Filename)) + "/_images/";

		FString UnrealTexturesPath = BaseDestPath + Mesh->Header->GameName + "Textures";
		for (size_t i = 0; i < Mesh->Materials.Num(); i++)
		{
			auto mat = Mesh->Materials[i];
			C2Material* CoDMaterial = new C2Material();
			CoDMaterial->Header->MaterialName = mat.MaterialName;
			for (size_t t = 0; t < mat.TextureNames.Num(); t++)
			{
				auto Texture = mat.TextureNames[t];
				C2MTexture CodTexture;
				CodTexture.TexturePath = Texture;
				CodTexture.TextureName = FPaths::GetCleanFilename(Texture).Replace(TEXT(".png"), TEXT(""));
				CodTexture.TextureType = mat.TextureTypes[t];

				FString FixedTexture = Texture.Replace(TEXT("\\_images\\"), TEXT(""));

				FString FixedImageFilePath = DiskTexturesPath + FixedTexture;

				if (!FPackageName::DoesPackageExist(FPaths::Combine(UnrealTexturesPath, C2MTexture::NoIllegalSigns(FixedTexture))) && FPaths::FileExists(FixedImageFilePath))
					CodTexture.TextureObject = ImportTexture(FixedImageFilePath, InParent);
				else
					UE_LOG(LogTemp, Log, TEXT("NICE"));
				CoDMaterial->Textures.Add(CodTexture);
			}
			C2Materials.Add(CoDMaterial);
		}
		//Mesh->Bones.Empty();
		if (Mesh->Bones.Num() > 1)
		{
			if (!FPackageName::DoesPackageExist(wepathPackage))
				MeshCreated = C2MStaticMesh::CreateSkeletalMesh(InParent, Mesh, C2Materials);
			   
		}
		else
		{
			if (!FPackageName::DoesPackageExist(wepathPackage))
				MeshCreated = C2MStaticMesh::CreateStaticMesh(InParent, Mesh, C2Materials);
		}
		// Parse C2M File Header

	}
	if (MeshCreated)
	{
		UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
		AssetEditorSubsystem->OpenEditorForAsset(MeshCreated);
	}

	return MeshCreated;
}

UObject* UC2ModelAssetFactory::ImportTexture(FString FilePath,UObject* InParent)
{

	//
	UAutomatedAssetImportData* importData = NewObject<UAutomatedAssetImportData>();
	importData->bReplaceExisting = true;
	// PathTextures
	FString ParentPath = FPaths::GetPath(InParent->GetPathName());
	FString MaterialsPath = FPaths::Combine(*ParentPath, TEXT("Materials"));
	FString TexturePath = FPaths::Combine(*MaterialsPath, TEXT("Textures"));
	importData->DestinationPath = TexturePath;
	
	importData->Filenames = TArray<FString>{ FilePath };
	FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
	if (!AssetToolsModule.Get().ImportAssetsAutomated(importData).IsValidIndex(0))
	{
		return nullptr;
	}
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
	return importedTexture;
}

