// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectFinderManager.h"
#include "Math/UnrealMathUtility.h"

TArray<class AObjectFinderManager*> AObjectFinderManagersHolder::m_objectsManagers;

void AObjectFinderManagersHolder::AddObjectManager(AObjectFinderManager* manager)
{
	if(!m_objectsManagers.Contains(manager))
	{
		m_objectsManagers.Add(manager);
	}
}

const TArray<AObjectFinderManager*>& AObjectFinderManagersHolder::GetObjectFinderManagers()
{
	return m_objectsManagers;
}

bool AObjectFinderManagersHolder::ManagersContainActor(const AActor* actor)
{
	if(m_objectsManagers.Num() <= 0)
	{
		return false;
	}
	for(const auto& objectManager : m_objectsManagers)
	{
		if(objectManager->m_randomActors.Contains(actor))
		{
			return true;
		}
	}
	return false;
}


void AObjectFinderManagersHolder::RemoveObjectManager(AObjectFinderManager* manager)
{
	if(m_objectsManagers.Contains(manager))
	{
		m_objectsManagers.Remove(manager);
	}
}

int32 AObjectFinderManagersHolder::GetTotalNumberOfRandomActors()
{
	if(m_objectsManagers.Num() <= 0)
	{
		return 0;
	}
	int32 totalNumberOfRandomActors = 0;
	for(const auto& manager : m_objectsManagers)
	{
		totalNumberOfRandomActors += manager->GetNumberOfRandomObjects();
	}
	return totalNumberOfRandomActors;
}


// Sets default values
AObjectFinderManager::AObjectFinderManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	m_totalNumberOfRandomObjects = 0;
}

// Called when the game starts or when spawned
void AObjectFinderManager::BeginPlay()
{
	Super::BeginPlay();
	AObjectFinderManagersHolder::AddObjectManager(this);
	GenerateRandomActors();
}

void AObjectFinderManager::BeginDestroy()
{
	Super::BeginDestroy();
	AObjectFinderManagersHolder::RemoveObjectManager(this);
}


void AObjectFinderManager::GenerateRandomActors()
{
	while(m_randomActors.Num() < GetNumberOfRandomObjects())
	{
		const FSearchedActor* generatedActor;
		do
		{
			generatedActor = GetRandomSearchedActor();
		}
		while (generatedActor == nullptr
			|| m_randomActors.Contains(generatedActor->m_actor));
		m_randomActors.Add(generatedActor->m_actor, const_cast<FSearchedActor*>(generatedActor));
	}
}

const FSearchedActor* AObjectFinderManager::GetRandomSearchedActor() const
{
	if(m_candidateActors.Num() <= 0)
	{
		return nullptr;
	}

	TArray<int32> searchedIndices;
	int32 randomIndex = -1;
	do
	{
		if(randomIndex != -1)
		{
			searchedIndices.Add(randomIndex);
		}
		randomIndex = FMath::RandRange(0, m_candidateActors.Num() - 1);
	}
	while (searchedIndices.Contains(randomIndex));
	return &m_candidateActors[randomIndex];
}


int32 AObjectFinderManager::GetNumberOfRandomObjects() const
{
	if(m_totalNumberOfRandomObjects <= 0)
	{
		return 0;
	}

	if(m_totalNumberOfRandomObjects > m_candidateActors.Num())
	{
		return m_candidateActors.Num();
	}
	return m_totalNumberOfRandomObjects;
}

// Called every frame
void AObjectFinderManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


