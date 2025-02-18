// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FExternFileInfo.h"
#include "FExternDirectoryInfo.h"
#include "FPatcherSpecifyAsset.h"

// RUNTIME
#include "FHotPatcherVersion.h"
#include "FChunkInfo.h"
#include "CreatePatch/FExportPatchSettings.h"
#include "ETargetPlatform.h"
// engine header
#include "Templates/SharedPointer.h"
#include "Dom/JsonObject.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FlibHotPatcherEditorHelper.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHotPatcherEditorHelper, Log, All);

struct FExportPatchSettings;
/**
 * 
 */
UCLASS()
class HOTPATCHEREDITOR_API UFlibHotPatcherEditorHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "HotPatch|Editor|Flib")
		static TArray<FString> GetAllCookOption();

	static void CreateSaveFileNotify(const FText& InMsg,const FString& InSavedFile,SNotificationItem::ECompletionState NotifyType = SNotificationItem::CS_Success);

	static void CheckInvalidCookFilesByAssetDependenciesInfo(const FString& InProjectAbsDir, const FString& InPlatformName, const FAssetDependenciesInfo& InAssetDependencies,TArray<FAssetDetail>& OutValidAssets,TArray<FAssetDetail>& OutInvalidAssets);

	static FChunkInfo MakeChunkFromPatchSettings(struct FExportPatchSettings const* InPatchSetting);
	static FChunkInfo MakeChunkFromPatchVerison(const FHotPatcherVersion& InPatchVersion);
	static FString GetCookAssetsSaveDir(const FString& BaseDir, const FString PacakgeName, const FString& Platform);

	static FString GetProjectCookedDir();

	//UFUNCTION(BlueprintCallable)
	static bool CookAssets(
			const TArray<FSoftObjectPath>& Assets,
			const TArray<ETargetPlatform>& Platforms,
			TFunction<void(const FString&)> PackageSavedCallback = [](const FString&){},
			class TMap<ETargetPlatform,FSavePackageContext*> PlatformSavePackageContext = TMap<ETargetPlatform,FSavePackageContext*>{}
		);
	static bool CookPackages(
		const TArray<FAssetData>& AssetDatas,
		const TArray<UPackage*>& Packages,
		const TArray<FString>& Platforms,
		TFunction<void(const FString&)> PackageSavedCallback = [](const FString&){},
		class TMap<FString,FSavePackageContext*> PlatformSavePackageContext = TMap<FString,FSavePackageContext*>{}
	);
	static bool CookPackage(
		const FAssetData& AssetData,
		UPackage* Package,
		const TArray<FString>& Platforms,
		TFunction<void(const FString&)> PackageSavedCallback = [](const FString&){},
		class TMap<FString,FSavePackageContext*> PlatformSavePackageContext = TMap<FString,FSavePackageContext*>{}
	);

	static void CookChunkAssets(
		TArray<FAssetDetail> Assets,
		const TArray<ETargetPlatform>& Platforms,
		class TMap<ETargetPlatform,FSavePackageContext*> PlatformSavePackageContext = TMap<ETargetPlatform,FSavePackageContext*>{}
	);
	
	static ITargetPlatform* GetTargetPlatformByName(const FString& PlatformName);
	static TArray<ITargetPlatform*> GetTargetPlatformsByNames(const TArray<ETargetPlatform>& PlatformNames);

	UFUNCTION(BlueprintCallable, Category = "HotPatcher|Editor|Flib")
    static FString GetUnrealPakBinary();
	UFUNCTION(BlueprintCallable, Category = "HotPatcher|Editor|Flib")
        static FString GetUECmdBinary();

	static FProcHandle DoUnrealPak(TArray<FString> UnrealPakCommandletOptions, bool block);

	static FString GetMetadataDir(const FString& ProjectDir,const FString& ProjectName,ETargetPlatform Platform);
	
	static void BackupMetadataDir(const FString& ProjectDir,const FString& ProjectName,const TArray<ETargetPlatform>& Platforms,const FString& OutDir);

	static FString ReleaseSummary(const FHotPatcherVersion& NewVersion);
	static FString PatchSummary(const FPatchVersionDiff& DiffInfo);

	static FString MakePakShortName(const FHotPatcherVersion& InCurrentVersion, const FChunkInfo& InChunkInfo, const FString& InPlatform,const FString& InRegular);

	static bool CheckSelectedAssetsCookStatus(const TArray<FString>& PlatformNames, const FAssetDependenciesInfo& SelectedAssets, FString& OutMsg);
	static bool CheckPatchRequire(const FPatchVersionDiff& InDiff,const TArray<FString>& PlatformNames,FString& OutMsg);

	// WindowsNoEditor to Windows
	static FString Conv2IniPlatform(const FString& Platform);
	static TArray<FString> GetSupportPlatforms();

	static FString GetEncryptSettingsCommandlineOptions(const FPakEncryptSettings& EncryptSettings,const FString& PlatformName);

	static ITargetPlatform* GetPlatformByName(const FString& Name);

	// need add UNREALED_API to FAssetRegistryGenerator
	// all chunksinfo.csv / pakchunklist.txt / assetregistry.bin
	static bool GeneratorAssetRegistryData(ITargetPlatform* TargetPlatform, const TSet<FName>&, const TSet<FName>&, bool bGenerateStreamingInstallManifest = true);

	/*
	* 0x1 Add
	* 0x2 Modyfy
	*/
	static void AnalysisWidgetTree(FPatchVersionDiff& PakDiff,int32 flags = 0x1|0x2);
	
	static FPatchVersionDiff DiffPatchVersionWithPatchSetting(const struct FExportPatchSettings& PatchSetting, const FHotPatcherVersion& Base, const FHotPatcherVersion& New);

	// CurrenrVersionChunk中的过滤器会进行依赖分析，TotalChunk的不会，目的是让用户可以自己控制某个文件夹打包到哪个Pak里，而不会对该文件夹下的资源进行依赖分析
	static FChunkAssetDescribe DiffChunkWithPatchSetting(
		const struct FExportPatchSettings& PatchSetting,
		const FChunkInfo& CurrentVersionChunk,
		const FChunkInfo& TotalChunk,
		TMap<FString, FAssetDependenciesInfo>& ScanedCaches
	);
	static FChunkAssetDescribe DiffChunkByBaseVersionWithPatchSetting(
		const struct FExportPatchSettings& PatchSetting,
		const FChunkInfo& CurrentVersionChunk,
		const FChunkInfo& TotalChunk,
		const FHotPatcherVersion& BaseVersion,
		TMap<FString, FAssetDependenciesInfo>& ScanedCaches
	);
	
	
	static FHotPatcherVersion MakeNewRelease(const FHotPatcherVersion& InBaseVersion, const FHotPatcherVersion& InCurrentVersion, FExportPatchSettings* InPatchSettings);
	static FHotPatcherVersion MakeNewReleaseByDiff(const FHotPatcherVersion& InBaseVersion, const FPatchVersionDiff& InDiff, FExportPatchSettings* InPatchSettings);

};
