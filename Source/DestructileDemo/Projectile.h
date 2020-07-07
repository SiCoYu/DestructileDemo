// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UProjectileMovementComponent;
class UCapsuleComponent;
class UStaticMeshComponent;
class UArrowComponent;

USTRUCT(BlueprintType)
struct FProjectileValues
{
	GENERATED_BODY()

	FProjectileValues()
	{
		BaseDamage = 20.0f;
		BaseVelocity = 200.0f;
		LifeTime = 5.0f;
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Projectile)
	float BaseDamage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Projectile)
	float BaseVelocity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Projectile, meta = (ClampMin = 0.0f))
	float LifeTime;
};


UCLASS()
class DESTRUCTILEDEMO_API AProjectile : public AActor
{
	GENERATED_BODY()
public:	
	AProjectile();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Projectile)
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Projectile)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Projectile)
	UCapsuleComponent* Collision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Projectile)
	FProjectileValues ProjectileValues;

	UPROPERTY(BlueprintReadOnly, Category = Projectile)
	float TimeSecondsWhenSpawned;
protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = Projectile)
	FVector GetImpulse() const;

	UFUNCTION(BlueprintCallable, Category = Projectile)
	void HandleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void HandleOnDestroyed(AActor* DestroyedActor);

	UFUNCTION(NetMulticast, Reliable)
	void ApplyDamage_Multicast(AActor* OtherActor, float Damage, const FHitResult& SweepResult);
};
