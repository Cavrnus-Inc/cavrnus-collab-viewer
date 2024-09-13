// Copyright(c) Cavrnus. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Actor.h"
#include "UI/CavrnusLoginWidget.h"
#include "UI/CavrnusGuestLoginWidget.h"
#include "UI/CavrnusSpaceListWidget.h"
#include "Types/CavrnusCallbackTypes.h"
#include "Types/CavrnusSpawnedObject.h"

#include "CavrnusSpatialConnector.generated.h"

/** Define various options for authentication methods, member and guest login methods, and space join methods used by the ACavrnusSpatialConnector class. */
UENUM()
enum class ECavrnusAuthMethod
{
	None,
	JoinAsMember,
	JoinAsGuest
};

UENUM()
enum class ECavrnusMemberLoginMethod
{
	None,
	EnterMemberLoginCredentials,
	PromptMemberToLogin
};

UENUM()
enum class ECavrnusGuestLoginMethod
{
	None,
	EnterNameBelow,
	PromptToEnterName
};

UENUM()
enum class ECavrnusSpaceJoinMethod
{
	None,
	EnterSpaceId,
	SpacesList
};


UCLASS()
class CAVRNUSCONNECTOR_API ACavrnusSpatialConnector : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACavrnusSpatialConnector();

	void CavrnusBeginPlay();
	void CavrnusEndPlay();

protected:
	/** Registers the Cavrnus subsystem proxy */
	virtual void PostInitializeComponents() override;

	/** Overridable native event for when play begins for this actor. */
	virtual void BeginPlay() override;

	/** Overridable function called whenever this actor is being removed from a level */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	/** Server address */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cavrnus")
	FString MyServer;

	// ------------------------------------------------------------ Authentication --------------------------------------------------------------------
	/** Property for selecting the authentication widget class. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cavrnus", META = (DisplayName = "Authentication Method"))
	ECavrnusAuthMethod AuthMethod;

	/** Enum property for selecting the guest login method. Editable only if AuthMethod is JoinAsGuest. Otherwise hidden. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cavrnus", META = (EditCondition = "AuthMethod == ECavrnusAuthMethod::JoinAsGuest", EditConditionHides))
	ECavrnusGuestLoginMethod GuestLoginMethod;

	/** Enum property for selecting the member login method. Editable only if AuthMethod is JoinAsMember. Otherwise hidden. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cavrnus", META = (EditCondition = "AuthMethod == ECavrnusAuthMethod::JoinAsMember", EditConditionHides))
	ECavrnusMemberLoginMethod MemberLoginMethod;

	/** Property for selecting a guest login widget class, editable only if AuthMethod is JoinAsGuest, and GuestLoginMethod is set to PromptToEnterName. Otherwise hidden */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cavrnus", META = (EditCondition = "AuthMethod == ECavrnusAuthMethod::JoinAsGuest && GuestLoginMethod == ECavrnusGuestLoginMethod::EnterNameBelow", EditConditionHides))
	FString GuestName;

	/** Property for selecting a guest login widget class, editable only if AuthMethod is JoinAsGuest, and GuestLoginMethod is set to PromptToEnterName. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cavrnus", META = (EditCondition = "AuthMethod == ECavrnusAuthMethod::JoinAsGuest && GuestLoginMethod == ECavrnusGuestLoginMethod::PromptToEnterName", EditConditionHides))
	TSubclassOf<UCavrnusGuestLoginWidget> GuestJoinMenu;

	/** Property for selecting member login email, editable only if AuthMethod is JoinAsMember, and MemberLoginMethod is set to EnterMemberLoginCredentials. Otherwise hidden */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cavrnus", META = (EditCondition = "AuthMethod == ECavrnusAuthMethod::JoinAsMember && MemberLoginMethod == ECavrnusMemberLoginMethod::EnterMemberLoginCredentials", EditConditionHides))
	FString MemberLoginEmail;

	/** String property for member login password, editable only if AuthMethod is JoinAsMember, and MemberLoginMethod is set to EnterMemberLoginCredentials. Otherwise hidden */
	UPROPERTY(EditAnywhere, Category = "Cavrnus", META = (PasswordField=true, EditCondition = "AuthMethod == ECavrnusAuthMethod::JoinAsMember && MemberLoginMethod == ECavrnusMemberLoginMethod::EnterMemberLoginCredentials", EditConditionHides))
	FString MemberLoginPassword;

	/** Property for selecting a member login widget class, editable only if AuthMethod is JoinAsMember, and MemberLoginMethod is set to PromptMemberToLogin. Otherwise hidden */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cavrnus", META = (EditCondition = "AuthMethod == ECavrnusAuthMethod::JoinAsMember && MemberLoginMethod == ECavrnusMemberLoginMethod::PromptMemberToLogin", EditConditionHides))
	TSubclassOf<UCavrnusLoginWidget> MemberLoginMenu;

	// --------------------------------------------------------- Space Joining ------------------------------------------------------------------
	/** Enum property for selecting the space join method. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cavrnus")
	ECavrnusSpaceJoinMethod SpaceJoinMethod;

	/** String property for the automatic space join ID, editable only if SpaceJoinMethod is EnterSpaceId. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cavrnus", META = (EditCondition = "SpaceJoinMethod == ECavrnusSpaceJoinMethod::EnterSpaceId", EditConditionHides))
	FString AutomaticSpaceJoinId;

	/** Property for selecting a space join menu widget class, editable only if SpaceJoinMethod is SpacesList. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cavrnus", META = (EditCondition = "SpaceJoinMethod == ECavrnusSpaceJoinMethod::SpacesList", EditConditionHides))
	TSubclassOf<UUserWidget> SpaceJoinMenu;

	// ------------------------------------------- Avatar Management ----------------------------------------------------------------------------
	/** Property for selecting the asset class type used to represent remote users. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, BlueprintReadOnly, Category = "Cavrnus")
	TSubclassOf<AActor> RemoteAvatarClass;

	// ------------------------------------------- In Space UI ----------------------------------------------------------------------------------
	/** Property for selecting the authentication widget class. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cavrnus")
	TSubclassOf<UUserWidget> AuthenticationWidgetClass;
	
	/** Property for selecting the loading widget class */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cavrnus")
	TSubclassOf<class UUserWidget> LoadingWidgetClass;

	/** Array property for specifying widgets to load. The array length is adjustable in the editor. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cavrnus")
	TArray<TSubclassOf<class UUserWidget>> WidgetsToLoad;

	/** Map property for specifying spawnable identifiers and their corresponding actor classes. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cavrnus")
	TMap<FString, TSubclassOf<AActor>> SpawnableIdentifiers;
};
