# Changelog

## [2024.3.1] - 2024-12-6

### Added
- SpaceInfo data added to CavrnusSpaceConnection
- Added JoinIdLogin widget to join space directly.
- Added ability to force/unforce RelayNet to Keep Alive while Unreal is frozen
- Changelog.md now copied to Docs subdirectory
- SpaceInformation UI button and window
- Server Selection Widget for case where Developer does not input server.
- Space Tags/Keywords
- Journal/local storage folder infrastructure 
- Token Caching to save Cavrnus login credentials

### Changed
- The "None" options in the CavrnusSpatialConnector menu now say "Custom", to reduce confusion.
- Plugin friendly name now shows as "Cavrnus Spatial Connector"
- Silent mode flag fixed and set to be True by default. Hides the Cavrnus Relay console window. Unhidden in project settings under Plugins / Cavrnus Spatial Connector.
- Cleanup of pawn sync components. Renamed several blueprint classes/components

### Removed
- Depedency on ProceduralMeshComponent plugin
- Unused pawn classes and blueprint assets

### Fixed
- Errors in SK_Avatar_CollabViewer_PhysicsAsset.uasset
- Miscellaneous Server Select menu fixes
- CSC no longer destroys the local user's pawn

## [2024.3.0] - 2024-10-11

### Added
- Create Space methods added to CavrnusFunctionLibrary and UI.
- New CavrnusSyncHeadRotation component for pawns.
- New Animation synching components - CavrnusPawnAnimationComponent and AC_CavrnusAnimationAccessor.uasset

### Changed
- Set EnabledByDefault to false in CavrnusConnector.uplugin. User needs to enable the plugin in the project, even when plugin is located in `<Project>/Plugins`.
- Animation accessor added to BP_BasePawn_Remote.uasset.
- Local property handling logic in CavrnusRelayNet.
- Increased transient property timeout period in CavrnusRelayNet.
- Performance improvements to interop batching.
- CavrnusRelayNet performance improvements - reduced logging, fixed scheduler.
- CavrnusConnectorSettings improvements.
  - Settings that apply to the project (e.g. RelayNetDisableRTC) should now go in DefaultCavrnus.ini. Copy the instance found in the plugin to `<Project>/Config`, and modify settings there to apply values in packaged builds.
  - Settings that are meant for local development (e.g. RelayNetVerboseLogging) can be set in the Unreal Editor menu Project | Plugins. These get stored in the `<Project>/Saved/Config/WindowsEditor` subdirectory.
- Moved CavrnusConnector loading to PreDefault loading phase. Fixes issues where adding CavrnusConnector uasset references to a C++ project's assets could result in failures on subsequent editor starts.

### Fixed
- Widget cleanup issues; moved one-time initialization from event Construct to OnInitialized. Added cleanup to NativeDestruct.
- Properties handling improvements to eliminate unnecessary callback execution.
- Bugs where property smoothing resulted in missing property updates.
- Data invalidation fix due to incorrect property change ID caching.
- Asset dependency errors in SK_Hand.uasset and SK_Hand_Skeleton.uasset.
- AC_Cavrnus_SyncAvatarTransform - IsValid checking to fix error on shutdown.
- Bug where multiple copies of UserWidget could appear in UserPanel.
- Sample Scene fixes and improvements.
- Bug where initial journal value callbacks would not fire.
