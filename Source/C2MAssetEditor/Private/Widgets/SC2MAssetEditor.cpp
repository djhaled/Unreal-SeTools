// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "SC2MAssetEditor.h"

#include "Misc/PackageName.h"
#include "C2MActor.h"
#include "C2MAsset.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/SBoxPanel.h"
#include "Misc/FileHelper.h"
#include "SC2MOptionsWindow.h"
#include "Shared.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "C2MModelInstance.h"
#include "EngineUtils.h"
#include "C2MAssetEditorSettings.h"
#include "C2Material.h"
#include "C2MMaterialInstance.h"
#include "Engine/Selection.h"
#include "EditorModeManager.h"
#include "LevelEditorViewport.h"


#define LOCTEXT_NAMESPACE "SC2MAssetEditor"
DEFINE_LOG_CATEGORY(LogC2M);

/* SC2MAssetEditor interface
 *****************************************************************************/

SC2MAssetEditor::~SC2MAssetEditor()
{
	FCoreUObjectDelegates::OnObjectPropertyChanged.RemoveAll(this);
}


void SC2MAssetEditor::Construct(const FArguments& InArgs, UC2MAsset* InC2MAsset, const TSharedRef<ISlateStyle>& InStyle)
{
	C2MAsset = InC2MAsset;
	CreateOptions = C2MAsset->CreateOptions;
	// Set Map Info
	CreateOptions->MapPath = InC2MAsset->Path;
	CreateOptions->MapName = InC2MAsset->Header->MapName;
	CreateOptions->GameVersion = InC2MAsset->Header->GameVersion;
	CreateOptions->MaterialCount = FString::FromInt(InC2MAsset->Header->MaterialsCount);
	CreateOptions->ModelCount = FString::FromInt(InC2MAsset->Header->ObjectsCount);
	CreateOptions->InstanceCount = FString::FromInt(InC2MAsset->Header->InstancesCount);
	CreateOptions->LightCount = FString::FromInt(InC2MAsset->Header->LightsCount);
	
	check(CreateOptions);
	auto Settings = GetDefault<UC2MAssetEditorSettings>();
	TSharedPtr<SBorder> MapHeaderDisplay;
	TSharedPtr<SBox> InspectorBox;
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Top)
		.HAlign(HAlign_Left)
		.Padding(2)
		[
			SNew(SUniformGridPanel)
			.SlotPadding(2)
			+ SUniformGridPanel::Slot(0, 0)
			[
				SNew(SButton)
				.VAlign(VAlign_Top)
				.HAlign(HAlign_Left)
				.Text(LOCTEXT("C2MOptionWindow_CreateMap", "Create Map"))
				.ToolTipText(LOCTEXT("C2MOptionWindow_CreateMap_ToolTip", "Create C2M level"))
				.OnClicked(this, &SC2MAssetEditor::OnCreateMap)
			]
			+ SUniformGridPanel::Slot(0, 1)
			[
				SNew(SButton)
				.VAlign(VAlign_Top)
				.HAlign(HAlign_Left)
				.Text(LOCTEXT("C2MOptionWindow_Test", "RotationTest0"))
				.ToolTipText(LOCTEXT("C2MOptionWindow_Test_ToolTip", "RotationTest0"))
				.OnClicked(this, &SC2MAssetEditor::OnButtonTest)
			]
			+ SUniformGridPanel::Slot(1, 1)
			[
				SNew(SButton)
				.VAlign(VAlign_Top)
				.HAlign(HAlign_Left)
				.Text(LOCTEXT("C2MOptionWindow_Test", "RotationTest1"))
				.ToolTipText(LOCTEXT("C2MOptionWindow_Test_ToolTip", "RotationTest1"))
				.OnClicked(this, &SC2MAssetEditor::OnButtonTest1)
			]
			+ SUniformGridPanel::Slot(2, 1)
			[
				SNew(SButton)
				.VAlign(VAlign_Top)
				.HAlign(HAlign_Left)
				.Text(LOCTEXT("C2MOptionWindow_Test", "RotationTest2"))
				.ToolTipText(LOCTEXT("C2MOptionWindow_Test_ToolTip", "RotationTest2"))
				.OnClicked(this, &SC2MAssetEditor::OnButtonTest2)
			]
			+ SUniformGridPanel::Slot(3, 1)
			[
				SNew(SButton)
				.VAlign(VAlign_Top)
				.HAlign(HAlign_Left)
				.Text(LOCTEXT("C2MOptionWindow_Test", "RotationTest3"))
				.ToolTipText(LOCTEXT("C2MOptionWindow_Test_ToolTip", "RotationTest3"))
				.OnClicked(this, &SC2MAssetEditor::OnButtonTest3)
			]
			+ SUniformGridPanel::Slot(4, 1)
			[
				SNew(SButton)
				.VAlign(VAlign_Top)
				.HAlign(HAlign_Left)
				.Text(LOCTEXT("C2MOptionWindow_Test", "RotationTest4"))
				.ToolTipText(LOCTEXT("C2MOptionWindow_Test_ToolTip", "RotationTest4"))
				.OnClicked(this, &SC2MAssetEditor::OnButtonTest4)
			]
			+ SUniformGridPanel::Slot(5, 1)
			[
				SNew(SButton)
				.VAlign(VAlign_Top)
				.HAlign(HAlign_Left)
				.Text(LOCTEXT("C2MOptionWindow_Test", "RotationTest5"))
				.ToolTipText(LOCTEXT("C2MOptionWindow_Test_ToolTip", "RotationTest5"))
				.OnClicked(this, &SC2MAssetEditor::OnButtonTest5)
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(2)
		[
			SAssignNew(InspectorBox, SBox)
			.MaxDesiredHeight(650.0f)
			.WidthOverride(400.0f)
		]
	];

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs(false, false, false, FDetailsViewArgs::HideNameArea);
	DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DetailsView->SetObject(CreateOptions);

	InspectorBox->SetContent(DetailsView->AsShared());
	
	//MapHeaderDisplay->SetContent(SC2MOptionsWindow::CreateMapHeader(InC2MAsset->Header).ToSharedRef());

	// FCoreUObjectDelegates::OnObjectPropertyChanged.AddSP(this, &SC2MAssetEditor::HandleTextAssetPropertyChanged);
}


/* STextAssetEditor callbacks
 *****************************************************************************/

// void SC2MAssetEditor::HandleEditableTextBoxTextChanged(const FText& NewText)
// {
// 	C2MAsset->MarkPackageDirty();
// }
//
//
// void SC2MAssetEditor::HandleEditableTextBoxTextCommitted(const FText& Comment, ETextCommit::Type CommitType)
// {
// 	C2MAsset->Path = FText::FromString(EditableTextBox->GetText();
// }
//
//
// void SC2MAssetEditor::HandleTextAssetPropertyChanged(UObject* Object, FPropertyChangedEvent& PropertyChangedEvent)
// {
// 	if (Object == C2MAsset)
// 	{
// 		EditableTextBox->SetText(C2MAsset->Path);
// 	}
// }

FReply SC2MAssetEditor::OnButtonTest()
{
	USelection* SelectedActors = GEditor->GetSelectedActors();
	TArray<AActor*> Actors;
	TArray<ULevel*> UniqueLevels;
	FVector4 baseRotation(0.56096744537353516f,-0.43047225475311279f,0.56096744537353516f, 0.43047225475311279f);
	for (FSelectionIterator Iter(*SelectedActors); Iter; ++Iter)
	{
		AActor* Actor = Cast<AActor>(*Iter);
		Actor->SetActorRotation(FRotator(FQuat(baseRotation.X, baseRotation.Y, baseRotation.Z, baseRotation.W)));
	}
	return FReply::Handled();
}
FReply SC2MAssetEditor::OnButtonTest1()
{
	USelection* SelectedActors = GEditor->GetSelectedActors();
	TArray<AActor*> Actors;
	TArray<ULevel*> UniqueLevels;
	FVector4 baseRotation(0.56096744537353516f,-0.43047225475311279f,0.56096744537353516f, 0.43047225475311279f);
	for (FSelectionIterator Iter(*SelectedActors); Iter; ++Iter)
	{
		AActor* Actor = Cast<AActor>(*Iter);
		Actor->SetActorRotation(FRotator(FQuat(baseRotation.X, baseRotation.Z, baseRotation.Y, baseRotation.W)));
	}
	return FReply::Handled();
}
TArray<C2MModelInstance*> SC2MAssetEditor::GetUniqueMergedModelInstances(TArray<C2MModelInstance*> OldInstances)
{
	TMap<FString, C2MModelInstance*> UniqueInstances;
	for (int m = 0; m < OldInstances.Num(); m++)
	{
		C2MModelInstance* ModelInstance = OldInstances[m];
		FString MIName = ModelInstance->Name;

		// Check if the instance already exists in the map
		if (UniqueInstances.Contains(MIName))
		{
			// If it does, add its transform to the existing instance
			UniqueInstances[MIName]->Transform.Add(ModelInstance->Transform[0]);
			delete ModelInstance; // Free up the memory used by the duplicate instance
		}
		else
		{
			// If it doesn't, add it to the map
			UniqueInstances.Add(MIName, ModelInstance);
		}

	}
	TArray< C2MModelInstance*> InstancesArrayReturn;
	UniqueInstances.GenerateValueArray(InstancesArrayReturn);
	return InstancesArrayReturn;
}
FReply SC2MAssetEditor::OnButtonTest2()
{
	USelection* SelectedActors = GEditor->GetSelectedActors();
	TArray<AActor*> Actors;
	TArray<ULevel*> UniqueLevels;
	FVector4 baseRotation(0.56096744537353516f,-0.43047225475311279f,0.56096744537353516f, 0.43047225475311279f);
	for (FSelectionIterator Iter(*SelectedActors); Iter; ++Iter)
	{
		AActor* Actor = Cast<AActor>(*Iter);
		Actor->SetActorRotation(FRotator(FQuat(baseRotation.Y, baseRotation.Z, baseRotation.X, baseRotation.W)));
	}
	return FReply::Handled();
}
FReply SC2MAssetEditor::OnButtonTest3()
{
	USelection* SelectedActors = GEditor->GetSelectedActors();
	TArray<AActor*> Actors;
	TArray<ULevel*> UniqueLevels;
	FVector4 baseRotation(0.56096744537353516f,-0.43047225475311279f,0.56096744537353516f, 0.43047225475311279f);
	for (FSelectionIterator Iter(*SelectedActors); Iter; ++Iter)
	{
		AActor* Actor = Cast<AActor>(*Iter);
		Actor->SetActorRotation(FRotator(FQuat(baseRotation.Y, baseRotation.X, baseRotation.Z, baseRotation.W)));
	}
	return FReply::Handled();
}

FReply SC2MAssetEditor::OnButtonTest4()
{
	USelection* SelectedActors = GEditor->GetSelectedActors();
	TArray<AActor*> Actors;
	TArray<ULevel*> UniqueLevels;
	FVector4 baseRotation(0.56096744537353516f,-0.43047225475311279f,0.56096744537353516f, 0.43047225475311279f);
	for (FSelectionIterator Iter(*SelectedActors); Iter; ++Iter)
	{
		AActor* Actor = Cast<AActor>(*Iter);
		Actor->SetActorRotation(FRotator(FQuat(baseRotation.Z, baseRotation.X, baseRotation.Y, baseRotation.W)));
	}
	return FReply::Handled();
}

FReply SC2MAssetEditor::OnButtonTest5()
{
	USelection* SelectedActors = GEditor->GetSelectedActors();
	TArray<AActor*> Actors;
	TArray<ULevel*> UniqueLevels;
	FVector4 baseRotation(0.56096744537353516f,-0.43047225475311279f,0.56096744537353516f, 0.43047225475311279f);
	for (FSelectionIterator Iter(*SelectedActors); Iter; ++Iter)
	{
		AActor* Actor = Cast<AActor>(*Iter);
		Actor->SetActorRotation(FRotator(FQuat(baseRotation.Z, baseRotation.Y, baseRotation.X, baseRotation.W)));
	}
	return FReply::Handled();
}
FReply SC2MAssetEditor::OnCreateMap()
{
	TArray64<uint8> FileData;
	if (FFileHelper::LoadFileToArray(FileData, *C2MAsset->Path))
	{
		FScopedSlowTask SlowTask(3, NSLOCTEXT("C2MAssetEditor", "BeginCreateC2MLevel", "Creating C2M Level"), true);
		SlowTask.MakeDialog(true);
		SlowTask.EnterProgressFrame(0);
		FLargeMemoryReader Reader(FileData.GetData(), FileData.Num());
		FString ModelsPath = FPaths::Combine(TEXT("/Game"),
											TEXT("C2M"),
											*C2MAsset->Header->GameVersion,
											TEXT("Models/"));
		FString MapGeoPath = FPaths::Combine(TEXT("/Game"),
											TEXT("C2M"),
											*C2MAsset->Header->GameVersion,
											TEXT("MapGeometry"),
											*C2MAsset->Header->MapName);
		
		TArray<UObject*> MeshAssets;
		EngineUtils::FindOrLoadAssetsByPath(MapGeoPath, MeshAssets, EngineUtils::ATL_Regular);
		// Creating the Actor and Positioning it in the World based in the Static Mesh
		auto myWorld = GEditor->GetLevelViewportClients()[0]->GetWorld();
		SlowTask.EnterProgressFrame(1, FText::FromString("Creating Map Geometry"));
		FFormatNamedArguments Args;
		Args.Add(TEXT("SurfaceName"), FText::FromString(""));
		Args.Add(TEXT("ModelName"), FText::FromString(""));
		FText MapGeoSlowTaskText = NSLOCTEXT("C2MAssetEditor", "C2MCreatingMapGeo", "{SurfaceName}");
		FScopedSlowTask MapGeoSlowTask(MeshAssets.Num(), FText::Format(MapGeoSlowTaskText, Args));
		MapGeoSlowTask.MakeDialog();
		for (auto& MapMeshAsset : MeshAssets)
		{
			Args[TEXT("SurfaceName")] = FText::FromString(MapMeshAsset->GetName());
			MapGeoSlowTask.EnterProgressFrame(1, FText::Format(MapGeoSlowTaskText, Args));
			UStaticMesh* MapStaticMesh = Cast<UStaticMesh>(MapMeshAsset);
			C2MActor::PlaceStaticMesh(MapStaticMesh, FTransform(), myWorld, MapStaticMesh->GetName(), true); // We set default transform for map geometry
		}

		if (CreateOptions->bCreateInstances)
		{
			int InstanceCount = C2MAsset->Header->InstancesCount;
			Reader.Seek(C2MAsset->Header->InstanceOffset);
			std::map<FString, AStaticMeshActor*> CreatedInstancesMap;
		
			SlowTask.EnterProgressFrame(1, FText::FromString("Creating Props"));
			FText ModelSlowTaskText = NSLOCTEXT("C2MAssetEditor", "C2MCreatingMapProps", "{ModelName}");
			FScopedSlowTask ModelSlowTask(InstanceCount, FText::Format(ModelSlowTaskText, Args));
			ModelSlowTask.MakeDialog();
			TArray< C2MModelInstance*> OldMI;
			for (int i = 0; i < InstanceCount; i++)
			{
				C2MModelInstance* ModelInstance = new C2MModelInstance();
				ModelInstance->ParseModelInstance(Reader);
				OldMI.Add(ModelInstance);
			}
			TArray< C2MModelInstance*> NewInstances;
			NewInstances = GetUniqueMergedModelInstances(OldMI);
			auto NewInstancesCount = NewInstances.Num();
			for (int i = 0; i < NewInstancesCount; i++)
			{
				C2MModelInstance* ModelInstance = NewInstances[i];
				Args[TEXT("ModelName")] = FText::FromString(ModelInstance->Name);
				ModelSlowTask.EnterProgressFrame(1, FText::Format(ModelSlowTaskText, Args));
				bool bRegisterMeshWithWorld = false;
				check(ModelInstance);
				FString ModelAssetPath = ModelsPath + ModelInstance->Name + TEXT(".") + ModelInstance->Name;
				// may need to fix this to check if its not 0.0.0 transform
				if (ModelInstance->Name.IsEmpty())
					continue;
				UStaticMesh* ModelAsset = LoadObject<UStaticMesh>(nullptr, *ModelAssetPath);
				if (IsValid(ModelAsset))
				{
					if (CreatedInstancesMap.find(ModelInstance->Name) == CreatedInstancesMap.end())
						bRegisterMeshWithWorld = true;
					if (ModelInstance->Transform.Num() == 1)
					{
						CreatedInstancesMap[ModelInstance->Name] = C2MActor::PlaceStaticMesh(ModelAsset, ModelInstance->Transform[0], myWorld, FString::FromInt(i) + TEXT("_") + ModelAsset->GetName(), bRegisterMeshWithWorld);
					}
					else
					{
						C2MActor::PlaceInstancedStaticMesh(ModelAsset, ModelInstance->Transform, myWorld, bRegisterMeshWithWorld);
					}
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("Model Missing: %s"), &ModelInstance->Name);
				}

			}
		}
		
		GLevelEditorModeTools().MapChangeNotify();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

#undef LOCTEXT_NAMESPACE
