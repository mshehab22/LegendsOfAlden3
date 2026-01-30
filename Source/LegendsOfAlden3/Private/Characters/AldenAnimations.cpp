#include "Characters/AldenAnimations.h"
#include "Characters/Alden.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UAldenAnimations::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Alden = Cast<AAlden>(TryGetPawnOwner());
	if (Alden)
	{
		AldenMovement = Alden->GetCharacterMovement();
	}
}

void UAldenAnimations::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (AldenMovement)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(AldenMovement->Velocity);

		IsFalling = AldenMovement->IsFalling();
	}
}
