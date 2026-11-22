// Blackline: Night Haul - pruebas automaticas (sin PIE).
// Ejecutar: UnrealEditor-Cmd.exe Blackline.uproject -ExecCmds="Automation RunTests Blackline; Quit" -unattended -nullrhi
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Cargo/CargoSimulationComponent.h"
#include "Checkpoint/CheckpointController.h"
#include "Core/BlacklineContractTypes.h"
#include "Engine/DataTable.h"
#include "UObject/Package.h"

namespace BlacklineTestHelpers
{
	UDataTable* LoadContracts()
	{
		return LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_BlacklineContracts.DT_BlacklineContracts"));
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBlacklineContractTableTest, "Blackline.Contracts.Table",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FBlacklineContractTableTest::RunTest(const FString&)
{
	UDataTable* Table = BlacklineTestHelpers::LoadContracts();
	if (!TestNotNull(TEXT("DT_BlacklineContracts existe"), Table))
	{
		return false;
	}
	TestEqual(TEXT("25 contratos"), Table->GetRowMap().Num(), 25);

	TMap<int32, int32> PerArc;
	Table->ForeachRow<FBlacklineContractRow>(TEXT("Test"), [this, &PerArc](const FName& Key, const FBlacklineContractRow& Row)
	{
		TestEqual(*FString::Printf(TEXT("%s: ContractID coincide con la fila"), *Key.ToString()), Row.ContractID, Key);
		TestTrue(*FString::Printf(TEXT("%s: fragilidad 0..1"), *Key.ToString()), Row.Fragility >= 0.f && Row.Fragility <= 1.f);
		TestTrue(*FString::Printf(TEXT("%s: EM 0..1"), *Key.ToString()), Row.EMSignature >= 0.f && Row.EMSignature <= 1.f);
		TestTrue(*FString::Printf(TEXT("%s: nivel de documento 1..3"), *Key.ToString()), Row.RequiredManifestLevel >= 1 && Row.RequiredManifestLevel <= 3);
		TestTrue(*FString::Printf(TEXT("%s: peso y pago positivos"), *Key.ToString()), Row.CargoWeightKg > 0.f && Row.Payout > 0);
		TestFalse(*FString::Printf(TEXT("%s: titulo no vacio"), *Key.ToString()), Row.Title.IsEmpty());
		PerArc.FindOrAdd(Row.Arc)++;
	});
	for (int32 Arc = 1; Arc <= 5; ++Arc)
	{
		TestEqual(*FString::Printf(TEXT("Arco %d tiene 5 contratos"), Arc), PerArc.FindRef(Arc), 5);
	}

	// Datos clave del brief.
	if (const FBlacklineContractRow* Last = Table->FindRow<FBlacklineContractRow>(TEXT("C25"), TEXT("Test")))
	{
		TestEqual(TEXT("C25 paga 25000"), Last->Payout, 25000);
		TestEqual(TEXT("C25 pesa 1550 kg"), Last->CargoWeightKg, 1550.f);
		TestEqual(TEXT("C25 exige nivel 3"), Last->RequiredManifestLevel, 3);
	}
	else
	{
		AddError(TEXT("Falta la fila C25"));
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBlacklineManifestLevelTest, "Blackline.Checkpoint.ManifestLevel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FBlacklineManifestLevelTest::RunTest(const FString&)
{
	TestEqual(TEXT("calidad 0.0 -> nivel 1"), ACheckpointController::ManifestLevelFromQuality(0.f), 1);
	TestEqual(TEXT("calidad 0.33 -> nivel 1"), ACheckpointController::ManifestLevelFromQuality(0.33f), 1);
	TestEqual(TEXT("calidad 0.34 -> nivel 2"), ACheckpointController::ManifestLevelFromQuality(0.34f), 2);
	TestEqual(TEXT("calidad 0.66 -> nivel 2"), ACheckpointController::ManifestLevelFromQuality(0.66f), 2);
	TestEqual(TEXT("calidad 0.67 -> nivel 3"), ACheckpointController::ManifestLevelFromQuality(0.67f), 3);
	TestEqual(TEXT("calidad 1.0 -> nivel 3"), ACheckpointController::ManifestLevelFromQuality(1.f), 3);
	TestEqual(TEXT("fuera de rango se acota"), ACheckpointController::ManifestLevelFromQuality(5.f), 3);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBlacklineCargoContractTest, "Blackline.Cargo.LoadContract",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FBlacklineCargoContractTest::RunTest(const FString&)
{
	UCargoSimulationComponent* Cargo = NewObject<UCargoSimulationComponent>(GetTransientPackage());
	if (!TestNotNull(TEXT("componente creado"), Cargo))
	{
		return false;
	}

	FBlacklineContractRow Row;
	Row.ContractID = TEXT("TEST01");
	Row.CargoWeightKg = 800.f;
	Row.Fragility = 0.5f;
	Row.EMSignature = 0.6f;
	Row.LateralBias = 1.f;

	TestTrue(TEXT("LoadContract ok"), Cargo->LoadContract(Row));
	TestEqual(TEXT("masa de la carga"), Cargo->GetTotalCargoMassKg(), 800.f);
	TestEqual(TEXT("integridad inicial"), Cargo->GetCargoIntegrity01(), 1.f);
	TestEqual(TEXT("contrato activo"), Cargo->GetActiveContractId(), FName(TEXT("TEST01")));

	// CoM con sesgo +1: la carga queda a la derecha (Y > 0).
	TestTrue(TEXT("CoM desplazado a la derecha"), Cargo->GetCargoCenterOfMassLocalCm().Y > 0.f);

	// Sin blindaje la firma EM cuenta; con blindaje se anula.
	TestTrue(TEXT("firma EM sin blindar > 0"), Cargo->GetUnshieldedSignature01() > 0.f);
	Cargo->bCargoShielded = true;
	TestEqual(TEXT("firma EM con blindaje = 0"), Cargo->GetUnshieldedSignature01(), 0.f);

	Cargo->ClearCargo();
	TestEqual(TEXT("masa tras limpiar"), Cargo->GetTotalCargoMassKg(), 0.f);
	TestTrue(TEXT("contrato activo vacio"), Cargo->GetActiveContractId().IsNone());
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
