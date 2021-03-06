// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosives.h"
#include "Main.h"

AExplosives::AExplosives() {
	Damage = 15.f;
}



void AExplosives::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	UE_LOG(LogTemp, Warning, TEXT("AExplosives::OnOverlapBegin()"));
	if (OtherActor) {
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)Main->DecrementHealth(Damage);
		Destroy();
	}
}

void AExplosives::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	UE_LOG(LogTemp, Warning, TEXT("AExplosives::OnOverlapEnd()"));
}

