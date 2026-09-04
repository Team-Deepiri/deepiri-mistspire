#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MistspireEntitySubsystem.generated.h"

/** A lightweight entity in the Mistspire entity store (Godex / Godot ECS equivalent). */
USTRUCT(BlueprintType)
struct FMistspireEntity
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) int32 Id = INDEX_NONE;
	UPROPERTY(BlueprintReadOnly) FName Archetype = NAME_None;
	UPROPERTY(BlueprintReadOnly) FVector Location = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly) TMap<FName, float> Floats;
	UPROPERTY(BlueprintReadOnly) TMap<FName, FName> Tags;

	/** Backing actor when this entity is actor-bound (ghosts, pickups); may be null. */
	UPROPERTY(BlueprintReadOnly) TWeakObjectPtr<AActor> Actor;

	UPROPERTY(BlueprintReadOnly) float TimeToLive = -1.f;
};

/** Archetype/component store for high-density entities (ghosts, embers, pickups). */
UCLASS()
class MISTSPIRE_API UMistspireEntitySubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	/** Spawns an entity of an archetype; returns the entity id (INDEX_NONE on failure). */
	UFUNCTION(BlueprintCallable, Category = "Mistspire|ECS")
	int32 SpawnEntity(FName Archetype, const FVector& Location, AActor* BoundActor = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|ECS")
	void DestroyEntity(int32 EntityId);

	/** Indices of all entities in an archetype (call GetEntity for details). */
	const TArray<int32>& QueryIds(FName Archetype) const;

	const FMistspireEntity* GetEntity(int32 EntityId) const;

	UFUNCTION(BlueprintCallable, Category = "Mistspire|ECS")
	void SetEntityFloat(int32 EntityId, FName Key, float Value);

	UFUNCTION(BlueprintPure, Category = "Mistspire|ECS")
	float GetEntityFloat(int32 EntityId, FName Key, float DefaultValue = 0.f) const;

	UFUNCTION(BlueprintCallable, Category = "Mistspire|ECS")
	void SetEntityTag(int32 EntityId, FName Key, FName Value);

	UFUNCTION(BlueprintPure, Category = "Mistspire|ECS")
	FName GetEntityTag(int32 EntityId, FName Key, FName DefaultValue = NAME_None) const;

	UFUNCTION(BlueprintCallable, Category = "Mistspire|ECS")
	void SetEntityLifetime(int32 EntityId, float Seconds);

	UFUNCTION(BlueprintPure, Category = "Mistspire|ECS")
	int32 GetEntityCount() const { return Entities.Num(); }

private:
	int32 NextEntityId = 1;
	TMap<int32, FMistspireEntity> Entities;
	TMap<FName, TArray<int32>> ArchetypeIndex;

	void RemoveFromIndex(FName Archetype, int32 EntityId);
};