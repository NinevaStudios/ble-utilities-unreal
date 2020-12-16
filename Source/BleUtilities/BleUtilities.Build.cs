// Copyright (c) 2019 Nineva Studios

using System.IO;
using UnrealBuildTool;

public class BleUtilities : ModuleRules
{
	public BleUtilities(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[]
			{
#if UE_4_20_OR_LATER
				Path.Combine (ModuleDirectory, "Public")
#else
				"BleUtilities/Public"
#endif
				// ... add other public include paths required here ...
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] {
#if UE_4_20_OR_LATER
				Path.Combine (ModuleDirectory, "Private")
#else
				"BleUtilities/Private"
#endif
				// ... add other private include paths required here ...
			}
		);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Engine",
				"Core",
				"CoreUObject"
				// ... add other public dependencies that you statically link with here ...
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects"
				// ... add private dependencies that you statically link with here ...	
			}
		);

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);

		if (Target.Platform == UnrealTargetPlatform.IOS)
		{
#if UE_4_20_OR_LATER
			PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/IOS"));
#else
			PrivateIncludePaths.AddRange(
				new string[]
				{
					"BleUtilities/Private/IOS"
					// ... add other private include paths required here ...
				}
			);
#endif
			PublicFrameworks.AddRange(
				new string[]
				{
                    "CoreBluetooth"
					// ... add other public frameworks required here ...
				}
			);

			PrivateDependencyModuleNames.AddRange(new string[] { "Launch" });
			string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);

#if UE_4_19_OR_LATER
			AdditionalPropertiesForReceipt.Add ("IOSPlugin", Path.Combine (PluginPath, "BleUtilitiesIos_UPL.xml"));
#else
			AdditionalPropertiesForReceipt.Add(new ReceiptProperty("IOSPlugin", Path.Combine(PluginPath, "BleUtilitiesIos_UPL.xml")));
#endif
		}

		// additional routine for Android
		if (Target.Platform == UnrealTargetPlatform.Android)
		{
#if UE_4_20_OR_LATER
			PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/Android"));
#else
			PrivateIncludePaths.AddRange(
				new string[]
				{
					"BleUtilities/Private/Android"
					// ... add other private include paths required here ...
				}
			);
#endif

			PrivateDependencyModuleNames.AddRange(new string[] { "Launch" });
			string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);

#if UE_4_19_OR_LATER
			AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(PluginPath, "BleUtilitiesAndroid_UPL.xml"));
#else
			AdditionalPropertiesForReceipt.Add(new ReceiptProperty("AndroidPlugin", Path.Combine(PluginPath, "BleUtilitiesAndroid_UPL.xml")));
#endif
		}
	}
}
