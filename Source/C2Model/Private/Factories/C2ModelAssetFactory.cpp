#include "Factories/C2ModelAssetFactory.h"
#include "Containers/UnrealString.h"
#include "Structures/C2Mesh.h"
#include "Misc/FileHelper.h"
#include "AssetToolsModule.h"
#include "Editor.h"
// widgets
#include "Widgets/Meshes/UserMeshOptions.h"
#include "Widgets/Meshes/SMeshImportOptions.h"
//
#include "Structures/C2Material.h"
#include "AssetTool/C2MStaticMesh.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Misc/ScopedSlowTask.h"
#include "Commandlets/ImportAssetsCommandlet.h"
#include "FileHelpers.h"
#include "IAutomationControllerManager.h"
#include "Interfaces/IMainFrameModule.h"
#include "Misc/FeedbackContext.h"
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
	UserSettings = CreateDefaultSubobject<UUserMeshOptions>(TEXT("Mesh Options"));
}


/* UFactory overrides
 *****************************************************************************/


UObject* UC2ModelAssetFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	// Load C2M File
	TArray64<uint8> FileDataOld;
	if (!FFileHelper::LoadFileToArray(FileDataOld, *Filename))
	{
		return nullptr;
	}
	// Create our base  Asset
	UObject* MeshCreated = nullptr;
	FString FileName_Fix = Filename.Replace(TEXT("_LOD0"),TEXT(""));
	FLargeMemoryReader Reader(FileDataOld.GetData(), FileDataOld.Num());
	C2Mesh* Mesh = new C2Mesh();
	Mesh->ParseMesh(Reader);
	Mesh->Header->MeshName =  FPaths::GetBaseFilename(FileName_Fix);
	if (!UserSettings->bInitialized)
	{
		TSharedPtr<SMeshImportOptions> ImportOptionsWindow;
		TSharedPtr<SWindow> ParentWindow;
		if (FModuleManager::Get().IsModuleLoaded("MainFrame"))
		{
			IMainFrameModule& MainFrame = FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
			ParentWindow = MainFrame.GetParentWindow();
		}

		TSharedRef<SWindow> Window = SNew(SWindow)
			.Title(FText::FromString(TEXT("SEModel Import Options")))
			.SizingRule(ESizingRule::Autosized);
		Window->SetContent
		(
			SAssignNew(ImportOptionsWindow, SMeshImportOptions)
			.WidgetWindow(Window)
			.MeshHeader(Mesh)
		);
		UserSettings = ImportOptionsWindow.Get()->Options;
		if (Mesh->Bones.Num() > 1)
		{
			UserSettings->MeshType = EMeshType::SkeletalMesh;
		}
		FSlateApplication::Get().AddModalWindow(Window, ParentWindow, false);
		bImport = ImportOptionsWindow.Get()->ShouldImport();
		bImportAll = ImportOptionsWindow.Get()->ShouldImportAll();
		UserSettings->bInitialized = true;
	}
	if (!UserSettings->bImportMaterials)
	{
		Mesh->Materials.Empty();
	}
	FString DiskTexturesPath = FPaths::GetPath(FPaths::GetPath(Filename)) + "/_images/";
	TArray<C2Material*> C2Materials;
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
			if (FPaths::FileExists(FixedImageFilePath))
			{
				CodTexture.TextureObject = ImportTexture(FixedImageFilePath, InParent);
			}
			else
				CodTexture.TextureObject = nullptr;
			CoDMaterial->Textures.Add(CodTexture);
		}
		C2Materials.Add(CoDMaterial);
	}
	// Model Stuff
	FString ModelPackage = FPaths::Combine(TEXT("/Game"), Mesh->Header->GameName, TEXT("Models"));

	C2MStaticMesh MeshBuildingClass;
	MeshBuildingClass.MeshOptions = UserSettings;
	if (UserSettings->bAutomaticallyDecideMeshType)
	{
		if (Mesh->Bones.Num() > 1)
		{
			UserSettings->MeshType = EMeshType::SkeletalMesh;
		}
	}

	MeshCreated = MeshBuildingClass.CreateMesh(InParent,ModelPackage,Mesh,C2Materials);

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
	const FString ParentPath = FPaths::GetPath(InParent->GetPathName());
	const FString MaterialsPath = FPaths::Combine(*ParentPath, TEXT("Materials"));
	FString TexturePath = FPaths::Combine(*MaterialsPath, TEXT("Textures"));
	const FString FileName = FPaths::GetCleanFilename(FilePath); // Extract just the filename from the path
	FString SanitizedFileName = C2MTexture::NoIllegalSigns(FileName); // Apply your sanitization function to the filename

	FString DestinationTexturePath = FPaths::Combine(TexturePath, SanitizedFileName); // Construct the destination path
	const FString NewDestFix = DestinationTexturePath.Replace(TEXT(".png"),TEXT(""));
	// Check if the texture already exists
	if (UTexture* LoadedTexture = LoadObject<UTexture>(nullptr, *NewDestFix))
	{
		// Texture already exists, return the loaded texture object
		return LoadedTexture;
	}

	// If the texture doesn't exist, you can proceed with importing it
	importData->DestinationPath = DestinationTexturePath;
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

