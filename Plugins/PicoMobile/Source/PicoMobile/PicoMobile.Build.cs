// Copyright Pico Technology Co, Ltd. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
namespace UnrealBuildTool.Rules
{
    public class PicoMobile : ModuleRules
    {
        public PicoMobile(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

            string ue4_include_path = Path.GetFullPath(Path.Combine(Target.RelativeEnginePath, "Source/Runtime/Renderer/Private"));
            string ue4_include_path2 = Path.GetFullPath(Path.Combine(Target.RelativeEnginePath, "Source/Runtime/Core/Private"));
            string ue4_include_path3 = Path.GetFullPath(Path.Combine(Target.RelativeEnginePath, "Source/Runtime/Launch/Private"));
            string ue4_include_path4 = Path.GetFullPath(Path.Combine(Target.RelativeEnginePath, "Source/Runtime/OpenGLDrv/Private"));
            PrivateIncludePaths.AddRange(
                new string[] {
                "PicoMobile/Private",
                ue4_include_path,
                ue4_include_path2,
                ue4_include_path3,
                ue4_include_path4
					// ... add other private include paths required here ...
				}
                );
           
            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "InputCore",
                    "RHI",
                    "RenderCore",
                    "Renderer",
                    //"ShaderCore",
                    "HeadMountedDisplay",
                    "Json",
                    "JsonUtilities"
					}
                );
  			PublicDependencyModuleNames.AddRange(new string[]
           {
                "EyeTracker"
           });

            PublicIncludePathModuleNames.Add("Launch");

            if (Target.bBuildEditor == true)
            {
                PrivateDependencyModuleNames.Add("UnrealEd");
            }

            PrivateDependencyModuleNames.AddRange(new string[] { "OpenGLDrv" });
            AddEngineThirdPartyPrivateStaticDependencies(Target, "OpenGL");

            if (Target.Platform == UnrealTargetPlatform.Android)
            {

                PrivateDependencyModuleNames.AddRange(new string[] { "Launch" });
                string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
                AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(PluginPath, "PicoMobileHMD_APL.xml"));
            }

        }
    }
}
