#include "MistspireEntitySubsystem.h"

TStatId UMistspireEntitySubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UMistspireEntitySubsystem, STATGROUP_Tickables);
}

int32 UMistspireEntitySubsystem::SpawnEntity(FName Archetype, const FVector& Location, AActor* BoundActor)
{
	if (Archetype == NAME_None)
	{
		return INDEX_NONE;
	}

	const int32 Id = NextEntityId++;
	FMistspireEntity Entity;
	Entity.Id = Id;
	Entity.Archetype = Archetype;
	Entity.Location = Location;
	Entity.Actor = BoundActor;

	Entities.Add(Id, Entity);
	ArchetypeIndex.FindOrAdd(Archetype).Add(Id);
	return Id;
}

void UMistspireEntitySubsystem::RemoveFromIndex(FName Archetype, int32 EntityId)
{
	if (TArray<int32>* Bucket = ArchetypeIndex.Find(Archetype))
	{
		Bucket->Remove(EntityId);
	}
}

void UMistspireEntitySubsystem::DestroyEntity(int32 EntityId)
{
	if (const FMistspireEntity* Entity = Entities.Find(EntityId))
	{
		RemoveFromIndex(Entity->Archetype, EntityId);
		Entities.Remove(EntityId);
	}
}

const TArray<int32>& UMistspireEntitySubsystem::QueryIds(FName Archetype) const
{
	static const TArray<int32> Empty;
	if (const TArray<int32>* Bucket = ArchetypeIndex.Find(Archetype))
	{
		return *Bucket;
	}
	return Empty;
}

const FMistspireEntity* UMistspireEntitySubsystem::GetEntity(int32 EntityId) const
{
	return Entities.Find(EntityId);
}

void UMistspireEntitySubsystem::SetEntityFloat(int32 EntityId, FName Key, float Value)
{
	if (FMistspireEntity* Entity = Entities.Find(EntityId))
	{
		Entity->Floats.FindOrAdd(Key) = Value;
	}
}

float UMistspireEntitySubsystem::GetEntityFloat(int32 EntityId, FName Key, float DefaultValue) const
{
	if (const FMistspireEntity* Entity = Entities.Find(EntityId))
	{
		if (const float* Value = Entity->Floats.Find(Key))
		{
			return *Value;
		}
	}
	return DefaultValue;
}

void UMistspireEntitySubsystem::SetEntityTag(int32 EntityId, FName Key, FName Value)
{
	if (FMistspireEntity* Entity = Entities.Find(EntityId))
	{
		Entity->Tags.FindOrAdd(Key) = Value;
	}
}

FName UMistspireEntitySubsystem::GetEntityTag(int32 EntityId, FName Key, FName DefaultValue) const
{
	if (const FMistspireEntity* Entity = Entities.Find(EntityId))
	{
		if (const FName* Value = Entity->Tags.Find(Key))
		{
			return *Value;
		}
	}
	return DefaultValue;
}

void UMistspireEntitySubsystem::SetEntityLifetime(int32 EntityId, float Seconds)
{
	if (FMistspireEntity* Entity = Entities.Find(EntityId))
	{
		Entity->TimeToLive = Seconds;
	}
}

void UMistspireEntitySubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<int32> ToExpire;
	for (TPair<int32, FMistspireEntity>& Pair : Entities)
	{
		FMistspireEntity& Entity = Pair.Value;
		if (Entity.TimeToLive >= 0.f)
		{
			Entity.TimeToLive -= DeltaTime;
			if (Entity.TimeToLive <= 0.f)
			{
				ToExpire.Add(Pair.Key);
			}
		}
	}

	for (const int32 Id : ToExpire)
	{
		DestroyEntity(Id);
	}
}