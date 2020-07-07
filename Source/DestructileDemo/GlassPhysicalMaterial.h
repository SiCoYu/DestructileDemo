// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "GlassPhysicalMaterial.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup = Physics)
class DESTRUCTILEDEMO_API UGlassPhysicalMaterial : public UPhysicalMaterial
{
	GENERATED_BODY()
public:
	UGlassPhysicalMaterial();

	UPROPERTY(EditDefaultsOnly, Category = "Material", meta = (DisplayName = "Is destructable glass"))
	bool bIsDestructableGlass;

	UFUNCTION(BlueprintPure, Category = "Material")
	FORCEINLINE bool IsDestructableGlass() const { return bIsDestructableGlass; };
	
};
