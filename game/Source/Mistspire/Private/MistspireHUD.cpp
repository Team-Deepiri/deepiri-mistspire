#include "MistspireHUD.h"
#include "MistspireAltitudeDebugSubsystem.h"
#include "MistspireAltitudeSubsystem.h"
#include "MistspireBeaconSubsystem.h"
#include "MistspireEnvironmentSubsystem.h"
#include "MistspireInputMode.h"
#include "MistspireInteriorSubsystem.h"
#include "MistspireNarrativeSubsystem.h"
#include "MistspireVRPawn.h"
#include "MistspireWorldAtlasSubsystem.h"
#include "MistspireZoneSubsystem.h"
#include "CanvasItem.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"
#include "Fonts/SlateFontInfo.h"
#include "GameFramework/PlayerController.h"

namespace
{
	UFont* GetHudFontAsset()
	{
		static TWeakObjectPtr<UFont> CachedFont;
		if (!CachedFont.IsValid())
		{
			CachedFont = LoadObject<UFont>(nullptr, TEXT("/Engine/EngineFonts/Roboto.Roboto"));
		}
		return CachedFont.Get();
	}

	void DrawSlateText(UCanvas* Canvas, const FString& Text, float X, float Y, float FontSize,
		const FLinearColor& Color, bool bCentreX, bool bCentreY)
	{
		UFont* FontAsset = GetHudFontAsset();
		if (!FontAsset || !Canvas)
		{
			return;
		}

		const FSlateFontInfo FontInfo(FontAsset, FontSize);
		FCanvasTextItem Item(FVector2D(FMath::RoundToFloat(X), FMath::RoundToFloat(Y)), FText::FromString(Text), FontInfo, Color);
		Item.bCentreX = bCentreX;
		Item.bCentreY = bCentreY;
		Item.EnableShadow(FLinearColor(0.f, 0.f, 0.f, 0.65f), FVector2D(1.f, 1.f));
		Canvas->DrawItem(Item);
	}

	void DrawGameplayHud(UCanvas* Canvas, UWorld* World, const AMistspireVRPawn* Pawn)
	{
		if (!Canvas || !World)
		{
			return;
		}

		float Y = 24.f;
		constexpr float X = 24.f;
		constexpr float LineSize = 18.f;
		constexpr float LineStep = 34.f;

		if (UMistspireAltitudeDebugSubsystem::IsHudEnabled())
		{
			if (UMistspireAltitudeSubsystem* Alt = World->GetSubsystem<UMistspireAltitudeSubsystem>())
			{
				FString Line1 = FString::Printf(TEXT("ALT  %.0f m   BEST %.0f m"),
					Alt->GetCurrentAltitudeCm() / 100.f,
					Alt->GetPersonalBestAltitudeCm() / 100.f);

				if (UMistspireZoneSubsystem* Zone = World->GetSubsystem<UMistspireZoneSubsystem>())
				{
					Line1 += FString::Printf(TEXT("   [ %s ]"),
						*UMistspireZoneSubsystem::GetZoneDisplayName(Zone->GetCurrentZone()).ToString());
				}
				if (UMistspireWorldAtlasSubsystem* Atlas = World->GetSubsystem<UMistspireWorldAtlasSubsystem>())
				{
					Line1 += FString::Printf(TEXT("   { %s }"),
						*UMistspireWorldAtlasSubsystem::GetDistrictDisplayName(Atlas->GetCurrentDistrict()).ToString());
				}

				DrawSlateText(Canvas, Line1, X, Y, LineSize, FLinearColor(0.2f, 0.9f, 1.f), false, false);
				Y += LineStep;
			}

			if (UMistspireEnvironmentSubsystem* Env = World->GetSubsystem<UMistspireEnvironmentSubsystem>())
			{
				FString WeatherLine = FString::Printf(TEXT("Weather: %s"), *Env->GetWeatherDisplayName().ToString());
				DrawSlateText(Canvas, WeatherLine, X, Y, LineSize, FLinearColor(0.75f, 0.75f, 0.75f), false, false);
				Y += LineStep;
			}

			if (Pawn)
			{
				DrawSlateText(Canvas,
					FString::Printf(TEXT("STA %.0f%%  O2 %.0f%%  P %.2f atm"),
						100.f * Pawn->GetStaminaPercent(),
						100.f * Pawn->GetOxygenPercent(),
						Pawn->GetAtmosphericPressure()),
					X, Y, LineSize, FLinearColor(0.3f, 1.f, 0.4f), false, false);
				Y += LineStep;
			}

			if (UMistspireInteriorSubsystem* Interior = World->GetSubsystem<UMistspireInteriorSubsystem>())
			{
				if (Interior->IsInsideInterior())
				{
					DrawSlateText(Canvas,
						FString::Printf(TEXT("INTERIOR: %s"), *Interior->GetCurrentBuildingId().ToString()),
						X, Y, LineSize, FLinearColor(1.f, 0.9f, 0.2f), false, false);
					Y += LineStep;
				}
			}

			if (UMistspireBeaconSubsystem* Beacon = World->GetSubsystem<UMistspireBeaconSubsystem>())
			{
				const FMistspireBeaconTarget Target = Beacon->GetCachedBeacon();
				if (Target.bValid)
				{
					DrawSlateText(Canvas,
						FString::Printf(TEXT("Beacon: %.1f km  bearing %.0f deg"),
							Target.DistanceCm / 100000.f, Target.BearingDegrees),
						X, Y, LineSize, FLinearColor(1.f, 0.55f, 0.1f), false, false);
					Y += LineStep;
				}
			}

			if (UMistspireNarrativeSubsystem* Narr = World->GetSubsystem<UMistspireNarrativeSubsystem>())
			{
				if (Narr->HasActiveLine())
				{
					DrawSlateText(Canvas, Narr->GetActiveLine().ToString(), X, Y, LineSize,
						FLinearColor(1.f, 0.95f, 0.7f), false, false);
					Y += LineStep;
				}
			}
		}

		if (UMistspireAltitudeDebugSubsystem::IsControlsHintEnabled())
		{
			Y += 8.f;
			DrawSlateText(Canvas, FMistspireInputMode::GetNonVRControlsHint(), X, Y, 16.f,
				FLinearColor::White, false, false);
		}
	}
}

void AMistspireHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas || !FMistspireInputMode::IsNonVRMode(GetWorld()))
	{
		return;
	}

	const AMistspireVRPawn* Pawn = nullptr;
	if (const APlayerController* PC = GetOwningPlayerController())
	{
		Pawn = Cast<AMistspireVRPawn>(PC->GetPawn());
	}

	const float CX = Canvas->ClipX * 0.5f;
	const float CY = Canvas->ClipY * 0.5f;

	if (Pawn && !Pawn->HasGameplayStarted())
	{
		DrawSlateText(Canvas, TEXT("Mistspire"), CX, CY, 72.f, FLinearColor::White, true, true);
		DrawSlateText(Canvas, TEXT("Press any key to start"), CX, Canvas->ClipY - 72.f, 28.f,
			FLinearColor(1.f, 1.f, 1.f, 0.92f), true, true);
		return;
	}

	if (Pawn && Pawn->IsSettingsMenuOpen())
	{
		return;
	}

	DrawGameplayHud(Canvas, GetWorld(), Pawn);

	constexpr float Arm = 10.f;
	constexpr float Thickness = 1.5f;
	const FLinearColor Color = FLinearColor(1.f, 1.f, 1.f, 0.85f);
	DrawLine(CX - Arm, CY, CX + Arm, CY, Color, Thickness);
	DrawLine(CX, CY - Arm, CX, CY + Arm, Color, Thickness);
}
