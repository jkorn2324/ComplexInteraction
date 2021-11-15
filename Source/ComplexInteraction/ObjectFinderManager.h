// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include <vector>

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectFinderManager.generated.h"

USTRUCT(BlueprintType)
struct COMPLEXINTERACTION_API FSearchedActor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	class AActor* m_actor;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString m_actorClue;
};

class AObjectFinderManagersHolder
{
public:
	static void AddObjectManager(class AObjectFinderManager* manager);
	static void RemoveObjectManager(class AObjectFinderManager* manager);

	static const TArray<class AObjectFinderManager*>& GetObjectFinderManagers();

	static bool ManagersContainActor(const AActor* actors);
	static int32 GetTotalNumberOfRandomActors();
	
private:
	static TArray<class AObjectFinderManager*> m_objectsManagers;
};

UCLASS()
class COMPLEXINTERACTION_API AObjectFinderManager : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AObjectFinderManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable)
	void GenerateRandomActors();
	UFUNCTION(BlueprintCallable)
	int32 GetNumberOfRandomObjects() const;

private:
	const FSearchedActor* GetRandomSearchedActor() const;	

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 m_totalNumberOfRandomObjects;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FSearchedActor> m_candidateActors;

public:
	TMap<AActor*, FSearchedActor*> m_randomActors;
};