// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerObjectFinderComponent.h"

#include "DrawDebugHelpers.h"
#include "ObjectFinderManager.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"

// Sets default values for this component's properties
UPlayerObjectFinderComponent::UPlayerObjectFinderComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	m_totalTimeTaken = 0.0f;
	m_directionRaycastLength = 200.0f;
	m_directionStartLength = 20.0f;
}


// Called when the game starts
void UPlayerObjectFinderComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UPlayerObjectFinderComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                 FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(TickType != ELevelTick::LEVELTICK_PauseTick)
	{
		if(!FoundAllRandomActors())
		{
			m_totalTimeTaken += DeltaTime;
		}
	}
}

UCameraComponent* UPlayerObjectFinderComponent::GetCameraComponent()
{
	if(m_cameraComponentReference == nullptr)
	{
		m_cameraComponentReference = GetOwner()->FindComponentByClass<UCameraComponent>();
	}
	return m_cameraComponentReference;
}

float UPlayerObjectFinderComponent::GetFoundObjectsPercentage() const
{
	const int32 totalNumberOfObjectsToFind = GetTotalNumberOfRandomObjectsInManagers();
	if(totalNumberOfObjectsToFind <= 0)
	{
		return 0.0f;
	}
	const int32 currentNumberOfObjects = GetNumberOfActorsFound();
	return static_cast<float>(currentNumberOfObjects) / static_cast<float>(totalNumberOfObjectsToFind);
}

bool UPlayerObjectFinderComponent::GetRandomSearchedActor(FSearchedActor& actor) const
{
	const int32 totalNumberOfObjectsToFind = GetTotalNumberOfRandomObjectsInManagers();
	if(totalNumberOfObjectsToFind <= 0)
	{
		return false;
	}
	const TArray<class AObjectFinderManager*>& objectManagers = AObjectFinderManagersHolder::GetObjectFinderManagers();
	if(objectManagers.Num() <= 0)
	{
		return false;
	}

	TArray<int32> objectManagersSearched;
	FSearchedActor* outputActor = nullptr;
	int32 randomInRange = -1;
	do
	{
		if(randomInRange >= 0)
		{
			objectManagersSearched.Add(randomInRange);			
		}
		randomInRange = FMath::RandRange(0, objectManagers.Num() - 1);
		AObjectFinderManager* randomManager = objectManagers[randomInRange];
		if(FoundAllActorsInManager(randomManager, outputActor))
		{
			continue;
		}
	} while(outputActor == nullptr
		&& objectManagersSearched.Num() < objectManagers.Num());

	if(outputActor == nullptr)
	{
		return false;
	}
	actor = *outputActor;
	return true;
}

bool UPlayerObjectFinderComponent::FoundAllActorsInManager(AObjectFinderManager* manager, FSearchedActor*& output) const
{
	if(manager->GetNumberOfRandomObjects() <= 0)
	{
		return true;
	}

	for(const auto& randomActor : manager->m_randomActors)
	{
		const auto outputFoundActor = HasFoundActor(randomActor.Key);
		if(outputFoundActor == RESULT_NOT_FOUND)
		{
			output = randomActor.Value;
			return false;
		}
	}
	return true;
}


TArray<AObjectFinderManager*> UPlayerObjectFinderComponent::GetObjectFinderManagers() const
{
	return AObjectFinderManagersHolder::GetObjectFinderManagers();
}

int32 UPlayerObjectFinderComponent::GetTotalNumberOfRandomObjectsInManagers() const
{
	return AObjectFinderManagersHolder::GetTotalNumberOfRandomActors();
}

float UPlayerObjectFinderComponent::GetTotalTimeTaken() const
{
	return m_totalTimeTaken;
}


int32 UPlayerObjectFinderComponent::GetNumberOfActorsFound() const
{
	return m_foundActors.Num();
}

bool UPlayerObjectFinderComponent::FoundAllRandomActors() const
{
	TArray<AObjectFinderManager*> objectFinderManagers
		= GetObjectFinderManagers();
	if(objectFinderManagers.Num() <= 0)
	{
		return true;
	}

	int32 calculateNumberOfRandomObjects = 0;
	for(const auto& objectManager : objectFinderManagers)
	{
		calculateNumberOfRandomObjects += objectManager->GetNumberOfRandomObjects();
	}
	return GetNumberOfActorsFound() >= calculateNumberOfRandomObjects;
}

void UPlayerObjectFinderComponent::FindSearchedActor()
{
	if(FoundAllRandomActors())
	{
		return;
	}
	FHitResult raycastHitResult;
	if(RaycastInOwnerDirection(raycastHitResult))
	{
		AActor* actorHit = raycastHitResult.GetActor();
		if(actorHit != nullptr)
		{
			const auto outputEnumerator = HasFoundActor(actorHit);
			if(outputEnumerator == UActorFinderResultType::RESULT_NOT_FOUND)
			{
				FoundActor(actorHit);
			}
		}
	}
}

void UPlayerObjectFinderComponent::FoundActor(AActor* actor)
{
	m_foundActors.Add(actor);
	m_playerFoundActorDelegate.Broadcast(this, actor);
	
	if(GetNumberOfActorsFound() >= GetTotalNumberOfRandomObjectsInManagers())
	{
		// TODO: Implementation - Player has found all actors and have won.
		return;
	}

	// TODO: Implementation
}


void UPlayerObjectFinderComponent::AddRaycastIgnoredObject(AActor* actor)
{
	if(!m_ignoredActors.Contains(actor))
	{
		m_ignoredActors.Add(actor);
	}
}

void UPlayerObjectFinderComponent::RemoveRaycastIgnoredObjectByIndex(int32 index)
{
	if(index >= 0
		&& index < m_ignoredActors.Num())
	{
		m_ignoredActors.RemoveAt(index);
	}
}


void UPlayerObjectFinderComponent::RemoveRaycastIgnoredObject(AActor* actor)
{
	if(m_ignoredActors.Contains(actor))
	{
		m_ignoredActors.Remove(actor);
	}
}

UActorFinderResultType UPlayerObjectFinderComponent::HasFoundActor(AActor* actor) const
{
	TArray<class AObjectFinderManager*> objectFinderManagers
		= AObjectFinderManagersHolder::GetObjectFinderManagers();
	if(objectFinderManagers.Num() <= 0)
	{
		return RESULT_UNKNOWN;
	}

	if(GetNumberOfActorsFound() >= GetTotalNumberOfRandomObjectsInManagers())
	{
		return RESULT_FOUND_ALL_ACTORS;
	}

	if(!AObjectFinderManagersHolder::ManagersContainActor(actor))
	{
		return RESULT_NOT_A_VALID_ACTOR;
	}
	if(!m_foundActors.Contains(actor))
	{
		return RESULT_NOT_FOUND;
	}
	return RESULT_FOUND;
}

const TArray<AActor*>& UPlayerObjectFinderComponent::GetIgnoredActors() const
{
	return m_ignoredActors;
}

bool UPlayerObjectFinderComponent::RaycastInOwnerDirection(FHitResult& outputResult)
{
	const FTransform& ownerTransform = GetOwner()->GetActorTransform();
	FVector startPos = ownerTransform.GetLocation();
	FVector forwardVector = GetOwner()->GetActorForwardVector();

	UCameraComponent* cameraComponent = GetCameraComponent();
	if(cameraComponent != nullptr)
	{
		forwardVector = cameraComponent->GetForwardVector();
	}
	startPos += forwardVector * m_directionStartLength;
	const FVector endPos = startPos
		+ forwardVector * m_directionRaycastLength;
	UWorld* currentWorld = GetWorld();
	if(currentWorld != nullptr)
	{
		FCollisionQueryParams queryParams;
		queryParams.AddIgnoredActors(m_ignoredActors);
		queryParams.AddIgnoredActor(GetOwner());
		
		return currentWorld->LineTraceSingleByChannel(outputResult,
				startPos, endPos,
				static_cast<ECollisionChannel>(
					ECollisionChannel::ECC_WorldDynamic | ECollisionChannel::ECC_WorldStatic),
				queryParams);
	}
	return false;
}


