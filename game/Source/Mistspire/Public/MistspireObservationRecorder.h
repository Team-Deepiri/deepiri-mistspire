#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "HAL/FileManager.h"
#include "MistspireObservationRecorder.generated.h"

/**
 * RL training telemetry (Godot RL Agents equivalent).
 * Samples a fixed observation vector and appends CSV rows to Saved/RL/observations.csv.
 * Train an agent on the rows offline; drive episodes with the debug console.
 */
UCLASS()
class MISTSPIRE_API UMistspireObservationRecorder : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	virtual void Deinitialize() override;
	virtual TStatId GetStatId() const override;

	/** Starts recording samples every SampleIntervalSeconds. */
	UFUNCTION(BlueprintCallable, Category = "Mistspire|RL")
	void StartRecording(float SampleIntervalSeconds = 1.f);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|RL")
	void StopRecording();

	UFUNCTION(BlueprintPure, Category = "Mistspire|RL")
	bool IsRecording() const { return bRecording; }

	UFUNCTION(BlueprintPure, Category = "Mistspire|RL")
	FString GetOutputPath() const;

	/** Appends one observation row immediately (used by console command too). */
	UFUNCTION(BlueprintCallable, Category = "Mistspire|RL")
	void SampleNow();

private:
	void CollectFeatures(TArray<float>& OutFeatures) const;
	void EnsureFileOpen();

	UPROPERTY(Transient)
	bool bRecording = false;

	float RecordingIntervalSeconds = 1.f;
	float Accumulator = 0.f;
	int32 RowsWritten = 0;

	IFileHandle* FileHandle = nullptr;
};