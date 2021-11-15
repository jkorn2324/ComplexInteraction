// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObjectFinderManager.h"
#include "Components/ActorComponent.h"
#include "Containers/Array.h"
#include "PlayerObjectFinderComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FPlayerFoundActorDelegate,
	class UPlayerObjectFinderComponent*, finderComponent,
	class AActor*, actor);

UENUM(Blueprintable)
enum UActorFinderResultType
{
	RESULT_FOUND,
	RESULT_NOT_FOUND,
	RESULT_NOT_A_VALID_ACTOR,
	RESULT_FOUND_ALL_ACTORS,
	RESULT_UNKNOWN
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COMPLEXINTERACTION_API UPlayerObjectFinderComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPlayerObjectFinderComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category="Object Finder Functions")
	void AddRaycastIgnoredObject(class AActor* actor);
	UFUNCTION(BlueprintCallable, Category="Object Finder Functions")
	void RemoveRaycastIgnoredObject(class AActor* actor);
	UFUNCTION(BlueprintCallable, Category="Object Finder Functions")
	void RemoveRaycastIgnoredObjectByIndex(int32 index);
	UFUNCTION(BlueprintCallable, Category="Object Finder Functions")
	bool RaycastInOwnerDirection(FHitResult& outputRaycast);

	UFUNCTION(BlueprintCallable, Category="Object Finder Functions")
	const TArray<class AActor*>& GetIgnoredActors() const;
	UFUNCTION(BlueprintCallable, Category="Object Finder Functions")
	bool FoundAllRandomActors() const;
	UFUNCTION(BlueprintCallable, Category="Object Finder Functions")
	UActorFinderResultType HasFoundActor(class AActor* actor) const;
	UFUNCTION(BlueprintCallable, Category="Object Finder Functions")
	void FindSearchedActor();
	UFUNCTION(BlueprintCallable, Category="Object Finder Functions")
	int32 GetNumberOfActorsFound() const;
	UFUNCTION(BlueprintCallable, Category="Object Finder Functions")
	float GetTotalTimeTaken() const;
	UFUNCTION(BlueprintCallable, Category="Object Finder Functions")
	TArray<class AObjectFinderManager*> GetObjectFinderManagers() const;
	UFUNCTION(BlueprintCallable, Category="Object Finder Functions")
	int32 GetTotalNumberOfRandomObjectsInManagers() const;
	UFUNCTION(BlueprintCallable, Category="Object Finder Functions")
	float GetFoundObjectsPercentage() const;
	UFUNCTION(BlueprintCallable, Category="Object Finder Functions")
	bool GetRandomSearchedActor(FSearchedActor& actor) const; 

private:
	class UCameraComponent* GetCameraComponent();
	bool FoundAllActorsInManager(AObjectFinderManager* manager, FSearchedActor*& output) const;
	void FoundActor(class AActor* actor);

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float m_directionStartLength;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float m_directionRaycastLength;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<class AActor*> m_ignoredActors;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UCameraComponent* m_cameraComponentReference; 
#pragma region delegates

	UPROPERTY(BlueprintAssignable)
	FPlayerFoundActorDelegate m_playerFoundActorDelegate;

#pragma endregion
	
private:
	TArray<class AActor*> m_foundActors;
	float m_totalTimeTaken;
};
