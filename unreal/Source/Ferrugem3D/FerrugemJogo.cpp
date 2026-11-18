// Ferrugem 3D - (Nexvora)
// Coloque em Source/Ferrugem3D/ junto com FerrugemJogo.h.

#include "FerrugemJogo.h"

#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/Canvas.h"
#include "EngineUtils.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// =====================================================================
// TABELAS (copiadas do Ferrugem 2D)
// =====================================================================
// Ferrugem 3D - tabelas do jogo (maquinas, itens e receitas)
// Gerado automaticamente a partir do Ferrugem 2D (index.html), para os numeros serem os mesmos.
// Coloque este arquivo em Source/Ferrugem3D/, junto com FerrugemJogo.h e FerrugemJogo.cpp.



struct FFerMaqInfo
{
	const TCHAR* Id;
	const TCHAR* Nome;
	int32 Preco;     // preco base (sobe 5% a cada maquina igual)
	uint32 Cor;      // cor 0xRRGGBB
	int32 Nivel;     // nivel em que libera
	float Consumo;   // energia que gasta (kW)
	float Gera;      // energia que gera (kW)
};

struct FFerItemInfo
{
	const TCHAR* Id;
	const TCHAR* Nome;
	float Valor;     // quanto vale no mercado
	uint32 Cor;
};

struct FFerEntrada
{
	int32 Item;
	int32 Qtd;
};

struct FFerReceita
{
	int32 Maq;
	int32 NumEntra;
	FFerEntrada Entra[4];
	int32 Sai;
	int32 Qtd;
	float Tempo;     // segundos
};

namespace FerM
{
	enum : int32
	{
		conv,
		miner,
		furn,
		press,
		gen,
		store,
		sell,
		wire,
		solar,
		asm_,
		mill,
		elec,
		motf,
		wind,
		robf,
		pump,
		refin,
		batt,
		car,
		fusion,
		chem,
		tiref,
		roll,
		bikef,
		scootf,
		motof,
		truckf,
		dronef,
		busf,
		railf,
		trainf,
		shipy,
		planef,
		rocketf,
		NUM
	};
}

namespace FerI
{
	enum : int32
	{
		Nenhum = -1,
		oFe,
		oCu,
		coal,
		iFe,
		iCu,
		pFe,
		pCu,
		wCu,
		gear,
		circ,
		oSi,
		glass,
		steel,
		screen,
		motor,
		robot,
		oil,
		plastic,
		battery,
		ecar,
		oAl,
		iAl,
		pAl,
		rubber,
		tire,
		bike,
		scoot,
		moto,
		truck,
		drone,
		bus,
		rail,
		train,
		ship,
		plane,
		rocket,
		NUM
	};
}

static const FFerMaqInfo GFerMaq[FerM::NUM] =
{
	{ TEXT("conv"), TEXT("Esteira"), 3, 0x3A4452, 1, 0.0f, 0.0f },
	{ TEXT("miner"), TEXT("Mineradora"), 85, 0xC9772B, 1, 20.0f, 0.0f },
	{ TEXT("furn"), TEXT("Forno"), 130, 0xB8442B, 1, 30.0f, 0.0f },
	{ TEXT("press"), TEXT("Prensa"), 190, 0x3B82A6, 2, 40.0f, 0.0f },
	{ TEXT("gen"), TEXT("Gerador"), 100, 0x7EA03A, 1, 0.0f, 80.0f },
	{ TEXT("store"), TEXT("Armaz\u00e9m"), 70, 0x7B6FB0, 2, 0.0f, 0.0f },
	{ TEXT("sell"), TEXT("Mercado"), 50, 0x2F9E6A, 1, 0.0f, 0.0f },
	{ TEXT("wire"), TEXT("Trefiladora"), 160, 0xA8662F, 3, 30.0f, 0.0f },
	{ TEXT("solar"), TEXT("Painel solar"), 260, 0x2B5C9E, 4, 0.0f, 35.0f },
	{ TEXT("asm"), TEXT("Montadora"), 320, 0x7D56B5, 5, 60.0f, 0.0f },
	{ TEXT("mill"), TEXT("Sider\u00fargica"), 450, 0x7A3B3B, 6, 70.0f, 0.0f },
	{ TEXT("elec"), TEXT("Eletr\u00f4nica"), 600, 0x1F7A6A, 7, 80.0f, 0.0f },
	{ TEXT("motf"), TEXT("F\u00e1b. de motores"), 800, 0x56606E, 8, 100.0f, 0.0f },
	{ TEXT("wind"), TEXT("Turbina e\u00f3lica"), 500, 0x8AA0B4, 9, 0.0f, 60.0f },
	{ TEXT("robf"), TEXT("Linha de rob\u00f4s"), 1500, 0xB5562F, 10, 150.0f, 0.0f },
	{ TEXT("pump"), TEXT("Bomba de petr\u00f3leo"), 600, 0x4B4F63, 11, 40.0f, 0.0f },
	{ TEXT("refin"), TEXT("Refinaria"), 900, 0x3F6F8F, 11, 80.0f, 0.0f },
	{ TEXT("batt"), TEXT("F\u00e1b. de baterias"), 1400, 0x2F7D4A, 12, 100.0f, 0.0f },
	{ TEXT("car"), TEXT("F\u00e1b. de carros"), 3000, 0xA33B4D, 13, 200.0f, 0.0f },
	{ TEXT("fusion"), TEXT("Reator de fus\u00e3o"), 8000, 0x36307A, 14, 0.0f, 500.0f },
	{ TEXT("chem"), TEXT("Petroqu\u00edmica"), 2500, 0x5B4A7A, 15, 120.0f, 0.0f },
	{ TEXT("tiref"), TEXT("F\u00e1b. de pneus"), 3200, 0x44403C, 16, 120.0f, 0.0f },
	{ TEXT("roll"), TEXT("Laminadora"), 3500, 0x8A96A3, 17, 140.0f, 0.0f },
	{ TEXT("bikef"), TEXT("F\u00e1b. de bicicletas"), 4500, 0x2F8F83, 18, 120.0f, 0.0f },
	{ TEXT("scootf"), TEXT("F\u00e1b. de patinetes"), 5500, 0x3A7BD5, 19, 140.0f, 0.0f },
	{ TEXT("motof"), TEXT("F\u00e1b. de motos"), 7000, 0xC2410C, 20, 180.0f, 0.0f },
	{ TEXT("truckf"), TEXT("F\u00e1b. de caminh\u00f5es"), 9000, 0x6B7280, 21, 220.0f, 0.0f },
	{ TEXT("dronef"), TEXT("F\u00e1b. de drones"), 10000, 0x0E7490, 22, 160.0f, 0.0f },
	{ TEXT("busf"), TEXT("F\u00e1b. de \u00f4nibus"), 12000, 0xB45309, 23, 260.0f, 0.0f },
	{ TEXT("railf"), TEXT("F\u00e1b. de trilhos"), 13000, 0x57534E, 24, 200.0f, 0.0f },
	{ TEXT("trainf"), TEXT("F\u00e1b. de trens"), 18000, 0x1D4ED8, 26, 320.0f, 0.0f },
	{ TEXT("shipy"), TEXT("Estaleiro"), 24000, 0x0F766E, 28, 360.0f, 0.0f },
	{ TEXT("planef"), TEXT("F\u00e1b. de avi\u00f5es"), 32000, 0x64748B, 30, 420.0f, 0.0f },
	{ TEXT("rocketf"), TEXT("Base de foguetes"), 50000, 0x7C2D12, 33, 600.0f, 0.0f },
};

static const FFerItemInfo GFerItem[FerI::NUM] =
{
	{ TEXT("oFe"), TEXT("Min\u00e9rio de ferro"), 2.0f, 0xC98A6B },
	{ TEXT("oCu"), TEXT("Min\u00e9rio de cobre"), 2.0f, 0x4FC3A1 },
	{ TEXT("coal"), TEXT("Carv\u00e3o"), 1.0f, 0x9AA5B1 },
	{ TEXT("iFe"), TEXT("Lingote de ferro"), 8.0f, 0xE8B496 },
	{ TEXT("iCu"), TEXT("Lingote de cobre"), 9.0f, 0x7EE8C8 },
	{ TEXT("pFe"), TEXT("Chapa de ferro"), 22.0f, 0xD7DDE5 },
	{ TEXT("pCu"), TEXT("Chapa de cobre"), 25.0f, 0x5FF0D0 },
	{ TEXT("wCu"), TEXT("Fio de cobre"), 7.0f, 0xE8954A },
	{ TEXT("gear"), TEXT("Engrenagem"), 55.0f, 0xB9C2CC },
	{ TEXT("circ"), TEXT("Circuito"), 60.0f, 0x3FAE5A },
	{ TEXT("oSi"), TEXT("Areia"), 1.0f, 0xE6CF95 },
	{ TEXT("glass"), TEXT("Vidro"), 9.0f, 0xBFE6F2 },
	{ TEXT("steel"), TEXT("A\u00e7o"), 35.0f, 0x7F8B99 },
	{ TEXT("screen"), TEXT("Tela"), 140.0f, 0x2FD6E8 },
	{ TEXT("motor"), TEXT("Motor"), 200.0f, 0x9AA6B4 },
	{ TEXT("robot"), TEXT("Rob\u00f4"), 700.0f, 0xFF9F1C },
	{ TEXT("oil"), TEXT("Petr\u00f3leo"), 3.0f, 0x2A2238 },
	{ TEXT("plastic"), TEXT("Pl\u00e1stico"), 24.0f, 0x8FD8FF },
	{ TEXT("battery"), TEXT("Bateria"), 150.0f, 0x4ADE80 },
	{ TEXT("ecar"), TEXT("Carro el\u00e9trico"), 2200.0f, 0xE5484D },
	{ TEXT("oAl"), TEXT("Bauxita"), 2.0f, 0xC2566F },
	{ TEXT("iAl"), TEXT("Lingote de alum\u00ednio"), 12.0f, 0xD9DEE6 },
	{ TEXT("pAl"), TEXT("Chapa de alum\u00ednio"), 35.0f, 0xEEF2F7 },
	{ TEXT("rubber"), TEXT("Borracha"), 18.0f, 0x52525B },
	{ TEXT("tire"), TEXT("Pneu"), 110.0f, 0x27272A },
	{ TEXT("bike"), TEXT("Bicicleta"), 600.0f, 0x14B8A6 },
	{ TEXT("scoot"), TEXT("Patinete el\u00e9trico"), 750.0f, 0x60A5FA },
	{ TEXT("moto"), TEXT("Moto"), 1100.0f, 0xF97316 },
	{ TEXT("truck"), TEXT("Caminh\u00e3o"), 2200.0f, 0x9CA3AF },
	{ TEXT("drone"), TEXT("Drone"), 900.0f, 0x22D3EE },
	{ TEXT("bus"), TEXT("\u00d4nibus"), 2600.0f, 0xFACC15 },
	{ TEXT("rail"), TEXT("Trilho"), 160.0f, 0xA8A29E },
	{ TEXT("train"), TEXT("Trem"), 5200.0f, 0x3B82F6 },
	{ TEXT("ship"), TEXT("Navio"), 3200.0f, 0x2DD4BF },
	{ TEXT("plane"), TEXT("Avi\u00e3o"), 4800.0f, 0xE2E8F0 },
	{ TEXT("rocket"), TEXT("Foguete"), 9000.0f, 0xEF4444 },
};

static const FFerReceita GFerReceitas[] =
{
	{ FerM::furn, 1, { { FerI::oFe, 1 }, { FerI::Nenhum, 0 }, { FerI::Nenhum, 0 }, { FerI::Nenhum, 0 } }, FerI::iFe, 1, 2.0f },
	{ FerM::furn, 1, { { FerI::oCu, 1 }, { FerI::Nenhum, 0 }, { FerI::Nenhum, 0 }, { FerI::Nenhum, 0 } }, FerI::iCu, 1, 2.0f },
	{ FerM::furn, 1, { { FerI::oSi, 1 }, { FerI::Nenhum, 0 }, { FerI::Nenhum, 0 }, { FerI::Nenhum, 0 } }, FerI::glass, 1, 2.0f },
	{ FerM::furn, 1, { { FerI::oAl, 2 }, { FerI::Nenhum, 0 }, { FerI::Nenhum, 0 }, { FerI::Nenhum, 0 } }, FerI::iAl, 1, 3.0f },
	{ FerM::press, 1, { { FerI::iFe, 2 }, { FerI::Nenhum, 0 }, { FerI::Nenhum, 0 }, { FerI::Nenhum, 0 } }, FerI::pFe, 1, 3.0f },
	{ FerM::press, 1, { { FerI::iCu, 2 }, { FerI::Nenhum, 0 }, { FerI::Nenhum, 0 }, { FerI::Nenhum, 0 } }, FerI::pCu, 1, 3.0f },
	{ FerM::wire, 1, { { FerI::iCu, 1 }, { FerI::Nenhum, 0 }, { FerI::Nenhum, 0 }, { FerI::Nenhum, 0 } }, FerI::wCu, 2, 2.0f },
	{ FerM::asm_, 2, { { FerI::pFe, 1 }, { FerI::wCu, 3 }, { FerI::Nenhum, 0 }, { FerI::Nenhum, 0 } }, FerI::circ, 1, 4.0f },
	{ FerM::asm_, 1, { { FerI::pFe, 2 }, { FerI::Nenhum, 0 }, { FerI::Nenhum, 0 }, { FerI::Nenhum, 0 } }, FerI::gear, 1, 3.0f },
	{ FerM::mill, 2, { { FerI::iFe, 2 }, { FerI::coal, 1 }, { FerI::Nenhum, 0 }, { FerI::Nenhum, 0 } }, FerI::steel, 1, 4.0f },
	{ FerM::elec, 2, { { FerI::glass, 2 }, { FerI::circ, 1 }, { FerI::Nenhum, 0 }, { FerI::Nenhum, 0 } }, FerI::screen, 1, 5.0f },
	{ FerM::motf, 3, { { FerI::gear, 1 }, { FerI::wCu, 4 }, { FerI::steel, 1 }, { FerI::Nenhum, 0 } }, FerI::motor, 1, 6.0f },
	{ FerM::robf, 3, { { FerI::motor, 1 }, { FerI::circ, 2 }, { FerI::steel, 2 }, { FerI::Nenhum, 0 } }, FerI::robot, 1, 8.0f },
	{ FerM::refin, 1, { { FerI::oil, 2 }, { FerI::Nenhum, 0 }, { FerI::Nenhum, 0 }, { FerI::Nenhum, 0 } }, FerI::plastic, 1, 3.0f },
	{ FerM::batt, 2, { { FerI::pCu, 2 }, { FerI::plastic, 1 }, { FerI::Nenhum, 0 }, { FerI::Nenhum, 0 } }, FerI::battery, 1, 5.0f },
	{ FerM::car, 4, { { FerI::motor, 1 }, { FerI::battery, 2 }, { FerI::steel, 2 }, { FerI::screen, 1 } }, FerI::ecar, 1, 10.0f },
	{ FerM::chem, 1, { { FerI::oil, 2 }, { FerI::Nenhum, 0 }, { FerI::Nenhum, 0 }, { FerI::Nenhum, 0 } }, FerI::rubber, 1, 3.0f },
	{ FerM::tiref, 2, { { FerI::rubber, 2 }, { FerI::steel, 1 }, { FerI::Nenhum, 0 }, { FerI::Nenhum, 0 } }, FerI::tire, 1, 4.0f },
	{ FerM::roll, 1, { { FerI::iAl, 2 }, { FerI::Nenhum, 0 }, { FerI::Nenhum, 0 }, { FerI::Nenhum, 0 } }, FerI::pAl, 1, 3.0f },
	{ FerM::bikef, 3, { { FerI::steel, 1 }, { FerI::tire, 2 }, { FerI::gear, 2 }, { FerI::Nenhum, 0 } }, FerI::bike, 1, 6.0f },
	{ FerM::scootf, 3, { { FerI::pAl, 2 }, { FerI::battery, 1 }, { FerI::tire, 2 }, { FerI::Nenhum, 0 } }, FerI::scoot, 1, 6.0f },
	{ FerM::motof, 4, { { FerI::motor, 1 }, { FerI::tire, 2 }, { FerI::steel, 2 }, { FerI::battery, 1 } }, FerI::moto, 1, 8.0f },
	{ FerM::truckf, 3, { { FerI::motor, 2 }, { FerI::tire, 6 }, { FerI::steel, 6 }, { FerI::Nenhum, 0 } }, FerI::truck, 1, 12.0f },
	{ FerM::dronef, 4, { { FerI::plastic, 2 }, { FerI::battery, 1 }, { FerI::circ, 2 }, { FerI::motor, 1 } }, FerI::drone, 1, 7.0f },
	{ FerM::busf, 4, { { FerI::motor, 2 }, { FerI::tire, 6 }, { FerI::pAl, 6 }, { FerI::screen, 2 } }, FerI::bus, 1, 14.0f },
	{ FerM::railf, 1, { { FerI::steel, 3 }, { FerI::Nenhum, 0 }, { FerI::Nenhum, 0 }, { FerI::Nenhum, 0 } }, FerI::rail, 1, 4.0f },
	{ FerM::trainf, 4, { { FerI::motor, 4 }, { FerI::rail, 10 }, { FerI::pAl, 8 }, { FerI::circ, 6 } }, FerI::train, 1, 20.0f },
	{ FerM::shipy, 3, { { FerI::steel, 20 }, { FerI::motor, 4 }, { FerI::screen, 2 }, { FerI::Nenhum, 0 } }, FerI::ship, 1, 24.0f },
	{ FerM::planef, 4, { { FerI::pAl, 20 }, { FerI::motor, 4 }, { FerI::circ, 10 }, { FerI::screen, 4 } }, FerI::plane, 1, 28.0f },
	{ FerM::rocketf, 4, { { FerI::pAl, 30 }, { FerI::circ, 20 }, { FerI::battery, 10 }, { FerI::motor, 6 } }, FerI::rocket, 1, 40.0f },
};
static const int32 GFerNumReceitas = UE_ARRAY_COUNT(GFerReceitas);


// Minerios do mapa (mesma ordem do 2D): 1 ferro, 2 cobre, 3 carvao, 4 areia, 5 petroleo, 6 bauxita
static const int32 GFerMinerioItem[7] = { FerI::Nenhum, FerI::oFe, FerI::oCu, FerI::coal, FerI::oSi, FerI::oil, FerI::oAl };
static const uint32 GFerMinerioCor[7] = { 0x000000, 0xC9772B, 0x2FBF9B, 0x7D8794, 0xD9BD7A, 0x2A1F3D, 0xC2566F };

// XP para cada nivel (igual ao 2D: 40 niveis)
static TArray<int64> FerNiveis()
{
	TArray<int64> L = { 0, 150, 500, 1200, 2500, 5000, 9000, 15000, 24000, 36000, 52000, 72000, 100000, 140000, 200000 };
	while (L.Num() < 40)
	{
		L.Add(FMath::RoundToInt64(L.Last() * 1.32 / 1000.0) * 1000);
	}
	return L;
}

static FLinearColor FerCor(uint32 C)
{
	return FLinearColor(FColor((C >> 16) & 255, (C >> 8) & 255, C & 255));
}


// Cor em hexadecimal (igual ao site) convertida para a cor "linear" que a Unreal usa.
static FLinearColor Hex(const TCHAR* Codigo)
{
	return FLinearColor(FColor::FromHex(Codigo));
}

// Direcao -> passo na grade (0 = +X, 1 = +Y, 2 = -X, 3 = -Y)
static FIntPoint DirDelta(int32 D)
{
	static const int32 DX[4] = { 1, 0, -1, 0 };
	static const int32 DY[4] = { 0, 1, 0, -1 };
	return FIntPoint(DX[D & 3], DY[D & 3]);
}

static FString NomeItem(int32 Item)
{
	return (Item >= 0 && Item < FerI::NUM) ? FString(GFerItem[Item].Nome) : FString(TEXT("?"));
}

static FString NomeMaq(int32 Tipo)
{
	return (Tipo >= 0 && Tipo < FerM::NUM) ? FString(GFerMaq[Tipo].Nome) : FString(TEXT("?"));
}

// Texto com as receitas de uma maquina, ex.: "Min. de ferro -> Lingote de ferro"
static FString ReceitasTexto(int32 Tipo)
{
	FString T;
	for (int32 r = 0; r < GFerNumReceitas; r++)
	{
		const FFerReceita& Rc = GFerReceitas[r];
		if (Rc.Maq != Tipo)
		{
			continue;
		}
		FString L;
		for (int32 e = 0; e < Rc.NumEntra; e++)
		{
			L += FString::Printf(TEXT("%s%d %s"), e ? TEXT(" + ") : TEXT(""), Rc.Entra[e].Qtd, *NomeItem(Rc.Entra[e].Item));
		}
		L += FString::Printf(TEXT("  ->  %d %s"), Rc.Qtd, *NomeItem(Rc.Sai));
		T += (T.IsEmpty() ? TEXT("") : TEXT("     |     ")) + L;
	}
	return T;
}

// =====================================================================
// MUNDO
// =====================================================================

AFerMundo::AFerMundo()
{
	PrimaryActorTick.bCanEverTick = true;

	// tudo "Movable": assim da para trocar formas e cores com o jogo rodando
	Raiz = CreateDefaultSubobject<USceneComponent>(TEXT("Raiz"));
	Raiz->SetMobility(EComponentMobility::Movable);
	RootComponent = Raiz;

	Chao = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Chao"));
	Chao->SetupAttachment(Raiz);
	Chao->SetMobility(EComponentMobility::Movable);
	Chao->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Linhas = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Linhas"));
	Linhas->SetupAttachment(Raiz);
	Linhas->SetMobility(EComponentMobility::Movable);
	Linhas->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// um conjunto de cristais para cada tipo de minerio (1 a 6)
	Cristais.SetNum(7);
	for (int32 k = 1; k <= 6; k++)
	{
		UInstancedStaticMeshComponent* C = CreateDefaultSubobject<UInstancedStaticMeshComponent>(*FString::Printf(TEXT("Cristais%d"), k));
		C->SetupAttachment(Raiz);
		C->SetMobility(EComponentMobility::Movable);
		C->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Cristais[k] = C;
	}
}

void AFerMundo::BeginPlay()
{
	Super::BeginPlay();

	const float Total = Lado * Tam;

	// chao escuro
	Chao->SetStaticMesh(Malha(TEXT("Plane")));
	Chao->SetMaterial(0, MaterialCor(this, Hex(TEXT("10151C"))));
	Chao->SetRelativeLocation(FVector(Total * 0.5f, Total * 0.5f, 0.5f));
	Chao->SetRelativeScale3D(FVector((float)Lado, (float)Lado, 1.f));

	// linhas da grade
	Linhas->SetStaticMesh(Malha(TEXT("Cube")));
	Linhas->SetMaterial(0, MaterialCor(this, Hex(TEXT("223040"))));
	for (int32 i = 0; i <= Lado; i++)
	{
		Linhas->AddInstance(FTransform(FRotator::ZeroRotator, FVector(i * Tam, Total * 0.5f, 1.5f), FVector(0.03f, Total / 100.f, 0.01f)));
		Linhas->AddInstance(FTransform(FRotator::ZeroRotator, FVector(Total * 0.5f, i * Tam, 1.5f), FVector(Total / 100.f, 0.03f, 0.01f)));
	}

	// cristais: cones coloridos (petroleo e uma mancha escura no chao)
	for (int32 k = 1; k <= 6; k++)
	{
		Cristais[k]->SetStaticMesh(Malha(k == 5 ? TEXT("Sphere") : TEXT("Cone")));
		Cristais[k]->SetMaterial(0, MaterialCor(this, FerCor(GFerMinerioCor[k])));
	}

	// jazidas (tipo, centro, raio): perto do comeco tem ferro, carvao e cobre
	CriarJazida(1, FIntPoint(16, 16), 4);  // ferro
	CriarJazida(3, FIntPoint(16, 26), 3);  // carvao
	CriarJazida(2, FIntPoint(27, 14), 4);  // cobre
	CriarJazida(4, FIntPoint(7, 8), 3);    // areia
	CriarJazida(1, FIntPoint(36, 30), 4);  // ferro
	CriarJazida(2, FIntPoint(26, 42), 3);  // cobre
	CriarJazida(3, FIntPoint(42, 14), 3);  // carvao
	CriarJazida(5, FIntPoint(46, 46), 3);  // petroleo
	CriarJazida(6, FIntPoint(12, 44), 3);  // bauxita
	CriarJazida(4, FIntPoint(50, 28), 3);  // areia
	CriarJazida(6, FIntPoint(52, 8), 3);   // bauxita
	CriarJazida(5, FIntPoint(5, 30), 3);   // petroleo

	// uma cor por item
	MatItens.SetNum(FerI::NUM);
	for (int32 i = 0; i < FerI::NUM; i++)
	{
		MatItens[i] = MaterialCor(this, FerCor(GFerItem[i].Cor));
	}

	// esconde o chao quadriculado do nivel "Basic" (o jogo tem o proprio chao)
	for (TActorIterator<AStaticMeshActor> It(GetWorld()); It; ++It)
	{
		if (It->GetName().Contains(TEXT("Floor")))
		{
			It->SetActorHiddenInGame(true);
		}
	}

	Avisar(TEXT("Comece: Gerador (5) + Mineradora (1) no carv\u00e3o, depois Mineradora no ferro + Forno + Mercado"), 8.f);
}

void AFerMundo::CriarJazida(int32 Tipo, FIntPoint C, int32 Raio)
{
	for (int32 dy = -Raio; dy <= Raio; dy++)
	{
		for (int32 dx = -Raio; dx <= Raio; dx++)
		{
			if (dx * dx + dy * dy > Raio * Raio)
			{
				continue;
			}
			const FIntPoint P(C.X + dx, C.Y + dy);
			if (!Dentro(P) || Minerio.Contains(P))
			{
				continue;
			}
			Minerio.Add(P, Tipo);
			if (Tipo == 5)
			{
				// petroleo: mancha achatada
				Cristais[Tipo]->AddInstance(FTransform(FRotator::ZeroRotator, Centro(P) + FVector(0.f, 0.f, 3.f), FVector(0.8f, 0.6f, 0.05f)));
			}
			else
			{
				const FVector Lugar = Centro(P) + FVector(FMath::FRandRange(-12.f, 12.f), FMath::FRandRange(-12.f, 12.f), 37.f);
				Cristais[Tipo]->AddInstance(FTransform(FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f), Lugar, FVector(0.45f, 0.45f, 0.75f)));
			}
		}
	}
}

void AFerMundo::Tick(float Dt)
{
	Super::Tick(Dt);

	// energia: soma o que e gerado e o que e pedido
	Sup = 0.f;
	Dem = 0.f;
	for (const TPair<FIntPoint, TWeakObjectPtr<AFerMaquina>>& Par : Maquinas)
	{
		const AFerMaquina* M = Par.Value.Get();
		if (!M)
		{
			continue;
		}
		const FFerMaqInfo& I = GFerMaq[M->Tipo];
		if (I.Gera > 0.f && M->Gerando())
		{
			Sup += I.Gera;
		}
		if (M->Quer)
		{
			Dem += I.Consumo;
		}
	}
	Sat = Dem > 0.f ? FMath::Min(1.f, Sup / Dem) : 1.f;

	// subiu de nivel?
	const int32 N = Nivel();
	if (N > NivelVisto)
	{
		FString Novas;
		for (int32 t = 0; t < FerM::NUM; t++)
		{
			if (GFerMaq[t].Nivel > NivelVisto && GFerMaq[t].Nivel <= N)
			{
				Novas += (Novas.IsEmpty() ? TEXT("") : TEXT(", ")) + NomeMaq(t);
			}
		}
		Avisar(FString::Printf(TEXT("N\u00edvel %d!%s%s"), N, Novas.IsEmpty() ? TEXT("") : TEXT("  Liberou: "), *Novas), 6.f);
		NivelVisto = N;
	}

	if (AvisoTempo > 0.f)
	{
		AvisoTempo -= Dt;
	}
}

FVector AFerMundo::Centro(FIntPoint P) const
{
	return FVector(P.X * Tam + Tam * 0.5f, P.Y * Tam + Tam * 0.5f, 0.f);
}

FIntPoint AFerMundo::Tile(const FVector& V) const
{
	return FIntPoint(FMath::FloorToInt(V.X / Tam), FMath::FloorToInt(V.Y / Tam));
}

bool AFerMundo::Dentro(FIntPoint P) const
{
	return P.X >= 0 && P.Y >= 0 && P.X < Lado && P.Y < Lado;
}

int32 AFerMundo::MinerioEm(FIntPoint P) const
{
	const int32* K = Minerio.Find(P);
	return K ? *K : 0;
}

AFerMaquina* AFerMundo::Em(FIntPoint P) const
{
	const TWeakObjectPtr<AFerMaquina>* W = Maquinas.Find(P);
	return (W && W->IsValid()) ? W->Get() : nullptr;
}

int32 AFerMundo::Nivel() const
{
	static const TArray<int64> L = FerNiveis();
	int32 N = 0;
	while (N < L.Num() && XP >= L[N])
	{
		N++;
	}
	return N;
}

int64 AFerMundo::XPDoNivel(int32 N) const
{
	static const TArray<int64> L = FerNiveis();
	return L.IsValidIndex(N) ? L[N] : L.Last();
}

float AFerMundo::VelEsteira() const
{
	const int32 N = Nivel();
	return N >= 12 ? 2.f : N >= 7 ? 1.5f : 1.f;
}

int32 AFerMundo::Contar(int32 Tipo) const
{
	int32 C = 0;
	for (const TPair<FIntPoint, TWeakObjectPtr<AFerMaquina>>& Par : Maquinas)
	{
		const AFerMaquina* M = Par.Value.Get();
		if (M && M->Tipo == Tipo)
		{
			C++;
		}
	}
	return C;
}

int32 AFerMundo::Preco(int32 Tipo) const
{
	const int32 Base = GFerMaq[Tipo].Preco;
	if (Tipo == FerM::conv)
	{
		return Base;
	}
	// cada maquina igual que voce ja tem deixa a proxima 5% mais cara
	return FMath::RoundToInt(Base * FMath::Pow(1.05f, (float)Contar(Tipo)));
}

FString AFerMundo::Motivo(int32 Tipo, FIntPoint P) const
{
	if (!Dentro(P))
	{
		return TEXT("Fora do mapa");
	}
	if (Em(P))
	{
		return TEXT("J\u00e1 tem uma m\u00e1quina aqui");
	}
	if (Nivel() < GFerMaq[Tipo].Nivel)
	{
		return FString::Printf(TEXT("%s libera no n\u00edvel %d"), *NomeMaq(Tipo), GFerMaq[Tipo].Nivel);
	}
	const int32 K = MinerioEm(P);
	if (Tipo == FerM::miner && (K == 0 || K == 5))
	{
		return TEXT("A mineradora precisa ficar em cima de um min\u00e9rio");
	}
	if (Tipo == FerM::pump && K != 5)
	{
		return TEXT("A bomba precisa ficar em cima do petr\u00f3leo (mancha escura)");
	}
	if (Dinheiro < Preco(Tipo))
	{
		return TEXT("Dinheiro insuficiente");
	}
	return FString();
}

FString AFerMundo::Construir(int32 Tipo, FIntPoint P, int32 Dir)
{
	const FString Erro = Motivo(Tipo, P);
	if (!Erro.IsEmpty())
	{
		return Erro;
	}
	const int32 Custo = Preco(Tipo);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AFerMaquina* M = GetWorld()->SpawnActor<AFerMaquina>(AFerMaquina::StaticClass(), Centro(P), FRotator(0.f, Dir * 90.f, 0.f), Params);
	if (!M)
	{
		return TEXT("Erro ao criar a m\u00e1quina");
	}
	M->Iniciar(this, Tipo, P, Dir);
	Maquinas.Add(P, M);
	Dinheiro -= Custo;
	return FString();
}

void AFerMundo::Remover(FIntPoint P)
{
	AFerMaquina* M = Em(P);
	if (!M)
	{
		return;
	}
	Dinheiro += GFerMaq[M->Tipo].Preco / 2;
	Maquinas.Remove(P);
	M->Destroy();
}

void AFerMundo::Vender(int32 Item)
{
	if (Item < 0 || Item >= FerI::NUM)
	{
		return;
	}
	// bonus de 3% por nivel, igual ao 2D
	const double V = GFerItem[Item].Valor * (1.0 + 0.03 * (Nivel() - 1));
	Dinheiro += V;
	XP += V;
	Vendidos++;
}

void AFerMundo::Avisar(const FString& Texto, float Tempo)
{
	Aviso = Texto;
	AvisoTempo = Tempo;
}

UStaticMesh* AFerMundo::Malha(const TCHAR* NomeMalha)
{
	const FString Caminho = FString::Printf(TEXT("/Engine/BasicShapes/%s.%s"), NomeMalha, NomeMalha);
	return LoadObject<UStaticMesh>(nullptr, *Caminho);
}

UStaticMesh* AFerMundo::ModeloDaMaquina(int32 Tipo)
{
	static TSet<int32> SemModelo;
	if (Tipo < 0 || Tipo >= FerM::NUM || SemModelo.Contains(Tipo))
	{
		return nullptr;
	}
	const FString Id = GFerMaq[Tipo].Id;
	const FString Caminho = FString::Printf(TEXT("/Game/Modelos/%s.%s"), *Id, *Id);
	UStaticMesh* M = LoadObject<UStaticMesh>(nullptr, *Caminho, nullptr, LOAD_NoWarn | LOAD_Quiet);
	if (!M)
	{
		SemModelo.Add(Tipo);
	}
	return M;
}

UMaterialInterface* AFerMundo::MaterialBase()
{
	return LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
}

UMaterialInstanceDynamic* AFerMundo::MaterialCor(UObject* Dono, FLinearColor C)
{
	UMaterialInstanceDynamic* M = UMaterialInstanceDynamic::Create(MaterialBase(), Dono);
	if (M)
	{
		M->SetVectorParameterValue(TEXT("Color"), C);
	}
	return M;
}

UMaterialInstanceDynamic* AFerMundo::MaterialDoItem(int32 Item)
{
	return MatItens.IsValidIndex(Item) ? MatItens[Item] : nullptr;
}

// =====================================================================
// MAQUINA
// =====================================================================

AFerMaquina::AFerMaquina()
{
	PrimaryActorTick.bCanEverTick = true;

	Raiz = CreateDefaultSubobject<USceneComponent>(TEXT("Raiz"));
	Raiz->SetMobility(EComponentMobility::Movable);
	RootComponent = Raiz;

	Corpo = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Corpo"));
	Corpo->SetupAttachment(Raiz);
	Corpo->SetMobility(EComponentMobility::Movable);
	Corpo->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Detalhe = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Detalhe"));
	Detalhe->SetupAttachment(Raiz);
	Detalhe->SetMobility(EComponentMobility::Movable);
	Detalhe->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Rotulo = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Rotulo"));
	Rotulo->SetupAttachment(Raiz);
	Rotulo->SetMobility(EComponentMobility::Movable);
	Rotulo->SetHorizontalAlignment(EHTA_Center);
	Rotulo->SetVerticalAlignment(EVRTA_TextCenter);
	Rotulo->SetWorldSize(15.f);
	Rotulo->SetTextRenderColor(FColor::White);
}

void AFerMaquina::Iniciar(AFerMundo* NovoMundo, int32 NovoTipo, FIntPoint NovaPos, int32 NovaDir)
{
	Mundo = NovoMundo;
	Tipo = NovoTipo;
	Pos = NovaPos;
	Girar(NovaDir);

	const FFerMaqInfo& Info = GFerMaq[Tipo];
	Corpo->SetMaterial(0, AFerMundo::MaterialCor(this, FerCor(Info.Cor)));

	// nome em cima da maquina, virado para a camera
	Rotulo->SetText(FText::FromString(Info.Nome));
	Rotulo->SetUsingAbsoluteRotation(true);
	Rotulo->SetWorldRotation(FRotator(55.f, 180.f, 0.f));
	Rotulo->SetRelativeLocation(FVector(0.f, 0.f, 120.f));

	// cor da "luz" = cor do item que a maquina faz
	CorLuz = Hex(TEXT("FFB020"));
	for (int32 r = 0; r < GFerNumReceitas; r++)
	{
		if (GFerReceitas[r].Maq == Tipo)
		{
			CorLuz = FerCor(GFerItem[GFerReceitas[r].Sai].Cor);
			break;
		}
	}

	FLinearColor CorDetalhe = Hex(TEXT("14090A"));
	Corpo->SetStaticMesh(AFerMundo::Malha(TEXT("Cube")));

	switch (Tipo)
	{
	case FerM::conv:
		// faixa baixa com uma seta amarela mostrando a direcao
		Corpo->SetRelativeScale3D(FVector(1.f, 0.8f, 0.1f));
		Corpo->SetRelativeLocation(FVector(0.f, 0.f, 5.f));
		Detalhe->SetStaticMesh(AFerMundo::Malha(TEXT("Cone")));
		Detalhe->SetRelativeScale3D(FVector(0.2f));
		Detalhe->SetRelativeLocation(FVector(30.f, 0.f, 15.f));
		Detalhe->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
		CorDetalhe = Hex(TEXT("E3A94F"));
		Rotulo->SetVisibility(false);
		break;

	case FerM::miner:
	case FerM::pump:
		// caixa com uma broca girando em cima
		Corpo->SetRelativeScale3D(FVector(0.8f, 0.8f, 0.6f));
		Corpo->SetRelativeLocation(FVector(0.f, 0.f, 30.f));
		Detalhe->SetStaticMesh(AFerMundo::Malha(TEXT("Cylinder")));
		Detalhe->SetRelativeScale3D(FVector(0.3f, 0.3f, 0.7f));
		Detalhe->SetRelativeLocation(FVector(0.f, 0.f, 75.f));
		CorDetalhe = Hex(TEXT("AAB3BD"));
		Minerio = GFerMinerioItem[Mundo->MinerioEm(Pos)];
		break;

	case FerM::gen:
		// gerador: cilindro verde com uma helice
		Corpo->SetStaticMesh(AFerMundo::Malha(TEXT("Cylinder")));
		Corpo->SetRelativeScale3D(FVector(0.8f, 0.8f, 0.6f));
		Corpo->SetRelativeLocation(FVector(0.f, 0.f, 30.f));
		Detalhe->SetRelativeScale3D(FVector(0.7f, 0.12f, 0.05f));
		Detalhe->SetRelativeLocation(FVector(0.f, 0.f, 65.f));
		Detalhe->SetStaticMesh(AFerMundo::Malha(TEXT("Cube")));
		CorDetalhe = Hex(TEXT("CDD8AE"));
		Estoque.Add(FerI::coal, 5); // comeca com um pouco de carvao, igual ao 2D
		break;

	case FerM::solar:
		// painel azul inclinado
		Corpo->SetRelativeScale3D(FVector(0.9f, 0.9f, 0.05f));
		Corpo->SetRelativeLocation(FVector(0.f, 0.f, 40.f));
		Corpo->SetRelativeRotation(FRotator(0.f, 0.f, 20.f));
		Detalhe->SetStaticMesh(AFerMundo::Malha(TEXT("Cylinder")));
		Detalhe->SetRelativeScale3D(FVector(0.08f, 0.08f, 0.4f));
		Detalhe->SetRelativeLocation(FVector(0.f, 0.f, 20.f));
		CorDetalhe = Hex(TEXT("8D99A6"));
		break;

	case FerM::wind:
		// torre alta com pas girando
		Corpo->SetStaticMesh(AFerMundo::Malha(TEXT("Cylinder")));
		Corpo->SetRelativeScale3D(FVector(0.15f, 0.15f, 2.f));
		Corpo->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
		Detalhe->SetStaticMesh(AFerMundo::Malha(TEXT("Cube")));
		Detalhe->SetRelativeScale3D(FVector(0.05f, 1.4f, 0.12f));
		Detalhe->SetRelativeLocation(FVector(-12.f, 0.f, 195.f));
		CorDetalhe = Hex(TEXT("F4F7FA"));
		Rotulo->SetRelativeLocation(FVector(0.f, 0.f, 240.f));
		break;

	case FerM::fusion:
		// reator: esfera roxa brilhante
		Corpo->SetRelativeScale3D(FVector(0.9f, 0.9f, 0.4f));
		Corpo->SetRelativeLocation(FVector(0.f, 0.f, 20.f));
		Detalhe->SetStaticMesh(AFerMundo::Malha(TEXT("Sphere")));
		Detalhe->SetRelativeScale3D(FVector(0.55f));
		Detalhe->SetRelativeLocation(FVector(0.f, 0.f, 65.f));
		CorDetalhe = Hex(TEXT("C4B5FD"));
		break;

	case FerM::sell:
		// caixa verde com uma moeda dourada girando
		Corpo->SetRelativeScale3D(FVector(0.85f, 0.85f, 0.5f));
		Corpo->SetRelativeLocation(FVector(0.f, 0.f, 25.f));
		Detalhe->SetStaticMesh(AFerMundo::Malha(TEXT("Cylinder")));
		Detalhe->SetRelativeScale3D(FVector(0.45f, 0.45f, 0.06f));
		Detalhe->SetRelativeLocation(FVector(0.f, 0.f, 80.f));
		Detalhe->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
		CorDetalhe = Hex(TEXT("F2C230"));
		break;

	case FerM::store:
		// armazem: caixa grande roxa
		Corpo->SetRelativeScale3D(FVector(0.9f, 0.9f, 0.8f));
		Corpo->SetRelativeLocation(FVector(0.f, 0.f, 40.f));
		Detalhe->SetStaticMesh(AFerMundo::Malha(TEXT("Cube")));
		Detalhe->SetRelativeScale3D(FVector(0.92f, 0.5f, 0.06f));
		Detalhe->SetRelativeLocation(FVector(0.f, 0.f, 82.f));
		CorDetalhe = Hex(TEXT("C4B5FD"));
		break;

	default:
		// fabricas: bloco colorido com uma "luz" em cima que acende ao trabalhar
		Corpo->SetRelativeScale3D(FVector(0.85f, 0.85f, 0.7f));
		Corpo->SetRelativeLocation(FVector(0.f, 0.f, 35.f));
		Detalhe->SetStaticMesh(AFerMundo::Malha(TEXT("Sphere")));
		Detalhe->SetRelativeScale3D(FVector(0.3f));
		Detalhe->SetRelativeLocation(FVector(0.f, 0.f, 82.f));
		break;
	}

	MatDetalhe = AFerMundo::MaterialCor(this, CorDetalhe);
	Detalhe->SetMaterial(0, MatDetalhe);

	// Modelo 3D proprio: se existir Content/Modelos/<id> (ex.: Modelos/miner), usa ele no lugar da caixa.
	// O tamanho e ajustado sozinho para caber no quadrado da grade, em pe no chao.
	if (UStaticMesh* Modelo = AFerMundo::ModeloDaMaquina(Tipo))
	{
		const FBox Caixa = Modelo->GetBoundingBox();
		const FVector Tamanho = Caixa.GetSize();
		const float Maior = FMath::Max3((float)Tamanho.X, (float)Tamanho.Y, 1.f);
		const float Escala = (Tipo == FerM::conv ? 100.f : 92.f) / Maior;
		const FVector Meio = Caixa.GetCenter();

		Corpo->SetStaticMesh(Modelo);
		Corpo->EmptyOverrideMaterials(); // usa as cores/texturas do proprio modelo
		Corpo->SetRelativeRotation(FRotator::ZeroRotator);
		Corpo->SetRelativeScale3D(FVector(Escala));
		Corpo->SetRelativeLocation(FVector(-Meio.X * Escala, -Meio.Y * Escala, -Caixa.Min.Z * Escala));

		// a esteira continua mostrando a seta; nas outras o modelo ja diz tudo
		if (Tipo != FerM::conv)
		{
			Detalhe->SetVisibility(false);
		}
		Rotulo->SetRelativeLocation(FVector(0.f, 0.f, Tamanho.Z * Escala + 25.f));
	}
}

void AFerMaquina::Girar(int32 NovaDir)
{
	Dir = NovaDir & 3;
	SetActorRotation(FRotator(0.f, Dir * 90.f, 0.f));
	if (Rotulo)
	{
		Rotulo->SetWorldRotation(FRotator(55.f, 180.f, 0.f));
	}
}

bool AFerMaquina::Gerando() const
{
	if (Tipo == FerM::gen)
	{
		return Queima > 0.f;
	}
	return GFerMaq[Tipo].Gera > 0.f;
}

int32 AFerMaquina::Total() const
{
	int32 T = 0;
	for (const TPair<int32, int32>& Par : Estoque)
	{
		T += Par.Value;
	}
	return T;
}

bool AFerMaquina::Aceita(int32 Item) const
{
	if (Item < 0 || Item >= FerI::NUM)
	{
		return false;
	}
	switch (Tipo)
	{
	case FerM::conv:
		return Itens.Num() < 4 && (Itens.Num() == 0 || Itens.Last().T >= 0.34f);
	case FerM::sell:
		return true;
	case FerM::store:
		return Total() < 200;
	case FerM::gen:
		return Item == FerI::coal && Estoque.FindRef(FerI::coal) < 20;
	default:
		break;
	}
	// fabricas: aceita se alguma receita usa esse item (estoque maximo: 12 ou o dobro da receita)
	for (int32 r = 0; r < GFerNumReceitas; r++)
	{
		const FFerReceita& Rc = GFerReceitas[r];
		if (Rc.Maq != Tipo)
		{
			continue;
		}
		for (int32 e = 0; e < Rc.NumEntra; e++)
		{
			if (Rc.Entra[e].Item == Item)
			{
				return Estoque.FindRef(Item) < FMath::Max(12, Rc.Entra[e].Qtd * 2);
			}
		}
	}
	return false;
}

void AFerMaquina::Receber(int32 Item)
{
	if (Tipo == FerM::conv)
	{
		FFerItemNaEsteira Novo;
		Novo.Item = Item;
		Novo.T = 0.f;
		Novo.Visual = CriarVisualItem(Item);
		Itens.Add(Novo);
		return;
	}
	if (Tipo == FerM::sell)
	{
		if (Mundo)
		{
			Mundo->Vender(Item);
		}
		return;
	}
	Estoque.FindOrAdd(Item)++;
}

UStaticMeshComponent* AFerMaquina::CriarVisualItem(int32 Item)
{
	UStaticMeshComponent* V = NewObject<UStaticMeshComponent>(this);
	V->SetMobility(EComponentMobility::Movable);
	const bool Bruto = Item == FerI::oFe || Item == FerI::oCu || Item == FerI::coal || Item == FerI::oSi || Item == FerI::oil || Item == FerI::oAl;
	V->SetStaticMesh(AFerMundo::Malha(Bruto ? TEXT("Sphere") : TEXT("Cube")));
	V->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	V->SetupAttachment(Raiz);
	V->SetRelativeScale3D(FVector(0.22f));
	V->SetRelativeLocation(FVector(-50.f, 0.f, 20.f));
	if (Mundo)
	{
		V->SetMaterial(0, Mundo->MaterialDoItem(Item));
	}
	V->RegisterComponent();
	return V;
}

void AFerMaquina::Luz(bool Acesa)
{
	const int32 Agora = Acesa ? 1 : 0;
	if (Agora == LuzVista || !MatDetalhe)
	{
		return;
	}
	LuzVista = Agora;
	MatDetalhe->SetVectorParameterValue(TEXT("Color"), Acesa ? CorLuz : Hex(TEXT("14090A")));
}

void AFerMaquina::Empurrar()
{
	if (Saida.Num() == 0 || !Mundo)
	{
		return;
	}
	AFerMaquina* Frente = Mundo->Em(Pos + DirDelta(Dir));
	if (Frente && Frente != this && Frente->Aceita(Saida[0]))
	{
		const int32 Item = Saida[0];
		Saida.RemoveAt(0);
		Frente->Receber(Item);
	}
}

void AFerMaquina::EmpurrarEstoque()
{
	AFerMaquina* Frente = Mundo ? Mundo->Em(Pos + DirDelta(Dir)) : nullptr;
	if (!Frente || Frente == this || Frente->Tipo == FerM::store)
	{
		return;
	}
	for (TPair<int32, int32>& Par : Estoque)
	{
		if (Par.Value > 0 && Frente->Aceita(Par.Key))
		{
			Par.Value--;
			Frente->Receber(Par.Key);
			return;
		}
	}
}

void AFerMaquina::Tick(float Dt)
{
	Super::Tick(Dt);
	if (!Mundo)
	{
		return;
	}
	Quer = false;

	switch (Tipo)
	{
	case FerM::conv:
		TickEsteira(Dt);
		return;

	case FerM::miner:
	case FerM::pump:
		TickMineradora(Dt);
		break;

	case FerM::gen:
		// queima 1 carvao a cada 10 segundos
		if (Queima <= 0.f && Estoque.FindRef(FerI::coal) > 0)
		{
			Estoque.FindOrAdd(FerI::coal)--;
			Queima = 10.f;
		}
		if (Queima > 0.f)
		{
			Queima -= Dt;
			Estado = 0;
			Detalhe->AddLocalRotation(FRotator(0.f, 540.f * Dt, 0.f));
		}
		else
		{
			Estado = 1;
		}
		break;

	case FerM::wind:
		Estado = 0;
		Detalhe->AddLocalRotation(FRotator(0.f, 0.f, 120.f * Dt));
		break;

	case FerM::solar:
	case FerM::fusion:
		Estado = 0;
		break;

	case FerM::sell:
		Estado = 0;
		Detalhe->AddWorldRotation(FRotator(0.f, 90.f * Dt, 0.f));
		break;

	case FerM::store:
		Estado = Total() > 0 ? 0 : 1;
		EmpurrarEstoque();
		break;

	default:
		TickReceita(Dt);
		break;
	}

	Empurrar();
}

void AFerMaquina::TickMineradora(float Dt)
{
	if (Minerio < 0)
	{
		Estado = 1;
		return;
	}
	if (Saida.Num() >= 4)
	{
		Estado = 3;
		return;
	}
	Quer = true;
	if (Mundo->Sat <= 0.f)
	{
		Estado = 2;
		return;
	}
	Estado = 0;
	Progresso += Dt * 0.7f * Mundo->Sat;
	if (Progresso >= 1.f)
	{
		Progresso -= 1.f;
		Saida.Add(Minerio);
	}
	Detalhe->AddLocalRotation(FRotator(0.f, 360.f * Mundo->Sat * Dt, 0.f));
}

void AFerMaquina::TickReceita(float Dt)
{
	// escolhe uma receita que tenha todos os ingredientes
	if (Receita < 0)
	{
		for (int32 r = 0; r < GFerNumReceitas; r++)
		{
			const FFerReceita& Rc = GFerReceitas[r];
			if (Rc.Maq != Tipo)
			{
				continue;
			}
			bool Tem = true;
			for (int32 e = 0; e < Rc.NumEntra; e++)
			{
				if (Estoque.FindRef(Rc.Entra[e].Item) < Rc.Entra[e].Qtd)
				{
					Tem = false;
					break;
				}
			}
			if (Tem)
			{
				for (int32 e = 0; e < Rc.NumEntra; e++)
				{
					Estoque.FindOrAdd(Rc.Entra[e].Item) -= Rc.Entra[e].Qtd;
				}
				Receita = r;
				Progresso = 0.f;
				break;
			}
		}
	}
	if (Receita < 0)
	{
		Estado = 1;
		Luz(false);
		return;
	}

	const FFerReceita& Rc = GFerReceitas[Receita];
	if (Progresso < Rc.Tempo)
	{
		Quer = true;
		if (Mundo->Sat > 0.f)
		{
			Progresso += Dt * Mundo->Sat;
			Estado = 0;
		}
		else
		{
			Estado = 2;
		}
	}
	if (Progresso >= Rc.Tempo)
	{
		if (Saida.Num() < 4)
		{
			for (int32 q = 0; q < Rc.Qtd; q++)
			{
				Saida.Add(Rc.Sai);
			}
			Receita = -1;
		}
		else
		{
			Estado = 3;
		}
	}
	Luz(Estado == 0);
}

void AFerMaquina::TickEsteira(float Dt)
{
	const float Vel = 1.2f * Mundo->VelEsteira();
	// os itens andam ate o fim, sem encostar um no outro
	for (int32 i = 0; i < Itens.Num(); i++)
	{
		const float Limite = (i == 0) ? 1.f : (Itens[i - 1].T - 0.34f);
		Itens[i].T = FMath::Max(Itens[i].T, FMath::Min(Itens[i].T + Dt * Vel, Limite));
		if (Itens[i].Visual)
		{
			Itens[i].Visual->SetRelativeLocation(FVector(-50.f + 100.f * Itens[i].T, 0.f, 20.f));
		}
	}
	// o primeiro item chegou no fim: passa para a maquina da frente
	if (Itens.Num() > 0 && Itens[0].T >= 1.f)
	{
		AFerMaquina* Frente = Mundo->Em(Pos + DirDelta(Dir));
		const bool Contramao = Frente && Frente->Tipo == FerM::conv && (Frente->Pos + DirDelta(Frente->Dir)) == Pos;
		if (Frente && Frente != this && !Contramao && Frente->Aceita(Itens[0].Item))
		{
			const int32 Item = Itens[0].Item;
			if (Itens[0].Visual)
			{
				Itens[0].Visual->DestroyComponent();
			}
			Itens.RemoveAt(0);
			Frente->Receber(Item);
		}
	}
}

FString AFerMaquina::EstadoTexto() const
{
	if (Tipo == FerM::conv)
	{
		return FString::Printf(TEXT("%d itens na esteira"), Itens.Num());
	}
	switch (Estado)
	{
	case 0:  return TEXT("Funcionando");
	case 1:  return Tipo == FerM::gen ? TEXT("Sem carv\u00e3o") : TEXT("Esperando material");
	case 2:  return TEXT("Sem energia!");
	case 3:  return TEXT("Sa\u00edda cheia (a frente est\u00e1 bloqueada)");
	default: return TEXT("");
	}
}

FString AFerMaquina::ConteudoTexto() const
{
	FString T;
	for (const TPair<int32, int32>& Par : Estoque)
	{
		if (Par.Value > 0)
		{
			T += FString::Printf(TEXT("%s: %d\n"), *NomeItem(Par.Key), Par.Value);
		}
	}
	if (Saida.Num() > 0)
	{
		T += FString::Printf(TEXT("Pronto para sair: %d %s\n"), Saida.Num(), *NomeItem(Saida[0]));
	}
	if (Receita >= 0)
	{
		T += FString::Printf(TEXT("Fazendo: %s (%d%%)\n"), *NomeItem(GFerReceitas[Receita].Sai), FMath::RoundToInt(100.f * Progresso / GFerReceitas[Receita].Tempo));
	}
	if (Tipo == FerM::gen && Queima > 0.f)
	{
		T += FString::Printf(TEXT("Queimando: %.0f s\n"), Queima);
	}
	return T;
}

// =====================================================================
// CAMERA
// =====================================================================

AFerCamera::AFerCamera()
{
	PrimaryActorTick.bCanEverTick = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	Raiz = CreateDefaultSubobject<USceneComponent>(TEXT("Raiz"));
	RootComponent = Raiz;

	Braco = CreateDefaultSubobject<USpringArmComponent>(TEXT("Braco"));
	Braco->SetupAttachment(Raiz);
	Braco->TargetArmLength = 1300.f;
	Braco->SetRelativeRotation(FRotator(-62.f, 0.f, 0.f));
	Braco->bDoCollisionTest = false;
	Braco->bEnableCameraLag = true;
	Braco->CameraLagSpeed = 8.f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(Braco, USpringArmComponent::SocketName);
}

// =====================================================================
// CONTROLE
// =====================================================================

AFerControle::AFerControle()
{
	bShowMouseCursor = true;
	bEnableClickEvents = false;
	bEnableMouseOverEvents = false;
}

void AFerControle::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameAndUI Modo;
	Modo.SetHideCursorDuringCapture(false);
	Modo.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(Modo);

	Ferramenta = FerM::miner;

	// "fantasma": mostra onde a maquina vai ser construida (verde = pode, vermelho = nao pode)
	MatFantasma = AFerMundo::MaterialCor(this, Hex(TEXT("22C55E")));
	Fantasma = CriarPeca(TEXT("Cube"), FVector(0.95f, 0.95f, 0.08f), MatFantasma);
	// seta branca: mostra para onde a maquina vai mandar os itens
	Seta = CriarPeca(TEXT("Cone"), FVector(0.25f), AFerMundo::MaterialCor(this, Hex(TEXT("F2F2F2"))));
}

AStaticMeshActor* AFerControle::CriarPeca(const TCHAR* NomeMalha, FVector Escala, UMaterialInstanceDynamic* Mat)
{
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AStaticMeshActor* A = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FTransform::Identity, Params);
	if (!A)
	{
		return nullptr;
	}
	UStaticMeshComponent* C = A->GetStaticMeshComponent();
	C->SetMobility(EComponentMobility::Movable);
	C->SetStaticMesh(AFerMundo::Malha(NomeMalha));
	C->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	C->SetMaterial(0, Mat);
	A->SetActorScale3D(Escala);
	A->SetActorHiddenInGame(true);
	return A;
}

void AFerControle::PlayerTick(float Dt)
{
	Super::PlayerTick(Dt);

	if (!Mundo)
	{
		for (TActorIterator<AFerMundo> It(GetWorld()); It; ++It)
		{
			Mundo = *It;
			break;
		}
		if (!Mundo)
		{
			return;
		}
	}

	// coloca a camera entre o ferro e o carvao do comeco
	if (!Posicionou && GetPawn())
	{
		GetPawn()->SetActorLocation(Mundo->Centro(FIntPoint(17, 20)));
		Posicionou = true;
	}

	MoverCamera(Dt);
	Teclas();
	AtualizarMira();
	Clicar();
}

void AFerControle::Teclas()
{
	// atalhos das maquinas basicas
	if (WasInputKeyJustPressed(EKeys::One))   Ferramenta = FerM::miner;
	if (WasInputKeyJustPressed(EKeys::Two))   Ferramenta = FerM::conv;
	if (WasInputKeyJustPressed(EKeys::Three)) Ferramenta = FerM::furn;
	if (WasInputKeyJustPressed(EKeys::Four))  Ferramenta = FerM::sell;
	if (WasInputKeyJustPressed(EKeys::Five))  Ferramenta = FerM::gen;

	// Q / E: passa por todas as 34 maquinas
	if (WasInputKeyJustPressed(EKeys::E)) Ferramenta = (Ferramenta + 1) % FerM::NUM;
	if (WasInputKeyJustPressed(EKeys::Q)) Ferramenta = (Ferramenta + FerM::NUM - 1) % FerM::NUM;

	// R: gira
	if (WasInputKeyJustPressed(EKeys::R)) Dir = (Dir + 1) % 4;
}

void AFerControle::MoverCamera(float Dt)
{
	AFerCamera* Cam = Cast<AFerCamera>(GetPawn());
	if (!Cam)
	{
		return;
	}

	// WASD (ou setas): a camera olha para +X, entao W = +X e D = +Y
	FVector Mov(0.f);
	if (IsInputKeyDown(EKeys::W) || IsInputKeyDown(EKeys::Up))    Mov.X += 1.f;
	if (IsInputKeyDown(EKeys::S) || IsInputKeyDown(EKeys::Down))  Mov.X -= 1.f;
	if (IsInputKeyDown(EKeys::D) || IsInputKeyDown(EKeys::Right)) Mov.Y += 1.f;
	if (IsInputKeyDown(EKeys::A) || IsInputKeyDown(EKeys::Left))  Mov.Y -= 1.f;
	const float Velocidade = Cam->Braco->TargetArmLength * 0.9f;
	Cam->AddActorWorldOffset(Mov.GetSafeNormal() * Velocidade * Dt);

	// zoom: roda do mouse, ou Z (aproxima) e X (afasta)
	float Zoom = GetInputAnalogKeyState(EKeys::MouseWheelAxis) * 250.f;
	if (WasInputKeyJustPressed(EKeys::MouseScrollUp))   Zoom += 250.f;
	if (WasInputKeyJustPressed(EKeys::MouseScrollDown)) Zoom -= 250.f;
	if (IsInputKeyDown(EKeys::Z)) Zoom += 1800.f * Dt;
	if (IsInputKeyDown(EKeys::X)) Zoom -= 1800.f * Dt;
	Cam->Braco->TargetArmLength = FMath::Clamp(Cam->Braco->TargetArmLength - Zoom, 400.f, 6000.f);
}

void AFerControle::AtualizarMira()
{
	MiraValida = false;

	// acha o ponto do chao (altura 0) que esta embaixo do mouse
	FVector Origem, Direcao;
	if (DeprojectMousePositionToWorld(Origem, Direcao) && Direcao.Z < -0.01f)
	{
		const FVector Ponto = Origem + Direcao * (-Origem.Z / Direcao.Z);
		Mira = Mundo->Tile(Ponto);
		MiraValida = Mundo->Dentro(Mira);
	}

	if (Fantasma)
	{
		Fantasma->SetActorHiddenInGame(!MiraValida);
		if (MiraValida)
		{
			Fantasma->SetActorLocation(Mundo->Centro(Mira) + FVector(0.f, 0.f, 6.f));
			const bool Pode = Mundo->Motivo(Ferramenta, Mira).IsEmpty();
			if (MatFantasma)
			{
				MatFantasma->SetVectorParameterValue(TEXT("Color"), Pode ? Hex(TEXT("22C55E")) : Hex(TEXT("EF4444")));
			}
		}
	}
	if (Seta)
	{
		Seta->SetActorHiddenInGame(!MiraValida);
		if (MiraValida)
		{
			Seta->SetActorLocationAndRotation(Mundo->Centro(Mira) + FVector(0.f, 0.f, 40.f), FRotator(-90.f, Dir * 90.f, 0.f));
		}
	}
}

void AFerControle::Clicar()
{
	const bool Segurando = IsInputKeyDown(EKeys::LeftMouseButton);
	if (!Segurando)
	{
		UltimoArrasto = FIntPoint(-999, -999);
	}
	if (!MiraValida)
	{
		return;
	}

	// botao direito: remove (devolve metade do preco)
	if (WasInputKeyJustPressed(EKeys::RightMouseButton))
	{
		Mundo->Remover(Mira);
		return;
	}
	if (!Segurando)
	{
		return;
	}

	const bool Novo = WasInputKeyJustPressed(EKeys::LeftMouseButton);

	// maquinas: um clique, uma maquina
	if (Ferramenta != FerM::conv)
	{
		if (Novo)
		{
			const FString Erro = Mundo->Construir(Ferramenta, Mira, Dir);
			if (!Erro.IsEmpty())
			{
				Mundo->Avisar(Erro);
			}
		}
		return;
	}

	// esteiras: segure e arraste; elas viram sozinhas na direcao do arrasto
	if (Mira == UltimoArrasto)
	{
		return;
	}
	const FIntPoint Passo = Mira - UltimoArrasto;
	if (!Novo && FMath::Abs(Passo.X) + FMath::Abs(Passo.Y) == 1)
	{
		Dir = Passo.X == 1 ? 0 : Passo.Y == 1 ? 1 : Passo.X == -1 ? 2 : 3;
		AFerMaquina* Anterior = Mundo->Em(UltimoArrasto);
		if (Anterior && Anterior->Tipo == FerM::conv)
		{
			Anterior->Girar(Dir);
		}
	}
	if (!Mundo->Em(Mira))
	{
		const FString Erro = Mundo->Construir(FerM::conv, Mira, Dir);
		if (!Erro.IsEmpty() && Novo)
		{
			Mundo->Avisar(Erro);
		}
	}
	UltimoArrasto = Mira;
}

// =====================================================================
// HUD
// =====================================================================

void AFerHUD::DrawHUD()
{
	Super::DrawHUD();

	AFerControle* PC = Cast<AFerControle>(GetOwningPlayerController());
	if (!PC || !PC->Mundo || !Canvas || !GEngine)
	{
		return;
	}
	AFerMundo* M = PC->Mundo;
	UFont* Fonte = GEngine->GetMediumFont();
	UFont* FonteGrande = GEngine->GetLargeFont();
	const float E = 1.2f; // tamanho dos textos
	const FLinearColor Laranja = Hex(TEXT("FF9F1C"));
	const FLinearColor Cinza = Hex(TEXT("B7C2CE"));
	const FLinearColor Fundo(0.f, 0.f, 0.f, 0.62f);

	// ---- painel de cima: dinheiro, nivel, energia
	const int32 N = M->Nivel();
	DrawRect(Fundo, 10.f, 10.f, 470.f, 132.f);
	DrawText(FString::Printf(TEXT("$ %s"), *FText::AsNumber(FMath::FloorToInt(M->Dinheiro)).ToString()), Laranja, 22.f, 14.f, FonteGrande, E);
	const FString TextoNivel = N >= 40
		? FString(TEXT("N\u00edvel 40 (m\u00e1ximo)"))
		: FString::Printf(TEXT("N\u00edvel %d    XP %s / %s"), N, *FText::AsNumber(FMath::FloorToInt(M->XP)).ToString(), *FText::AsNumber(M->XPDoNivel(N)).ToString());
	DrawText(TextoNivel, FLinearColor::White, 22.f, 58.f, Fonte, E);
	const bool Falta = M->Dem > M->Sup + 0.01f;
	DrawText(FString::Printf(TEXT("Energia: %.0f / %.0f kW%s"), M->Sup, M->Dem, Falta ? TEXT("   (FALTA ENERGIA!)") : TEXT("")), Falta ? Hex(TEXT("F87171")) : Hex(TEXT("BEF264")), 22.f, 84.f, Fonte, E);
	DrawText(FString::Printf(TEXT("Itens vendidos: %d"), M->Vendidos), Cinza, 22.f, 110.f, Fonte, E);

	// ---- painel da direita: o que tem embaixo do mouse
	if (PC->MiraValida)
	{
		FString Titulo, Corpo;
		if (AFerMaquina* Maq = M->Em(PC->Mira))
		{
			Titulo = FString(GFerMaq[Maq->Tipo].Nome);
			Corpo = Maq->EstadoTexto() + TEXT("\n") + Maq->ConteudoTexto();
		}
		else if (const int32 K = M->MinerioEm(PC->Mira))
		{
			Titulo = NomeItem(GFerMinerioItem[K]);
			Corpo = K == 5 ? TEXT("Use uma Bomba de petr\u00f3leo aqui") : TEXT("Use uma Mineradora aqui");
		}
		if (!Titulo.IsEmpty())
		{
			TArray<FString> Linhas;
			Corpo.ParseIntoArrayLines(Linhas);
			const float X0 = Canvas->ClipX - 430.f;
			DrawRect(Fundo, X0, 10.f, 420.f, 50.f + 26.f * Linhas.Num());
			DrawText(Titulo, Laranja, X0 + 12.f, 16.f, Fonte, E * 1.15f);
			for (int32 i = 0; i < Linhas.Num(); i++)
			{
				DrawText(Linhas[i], FLinearColor::White, X0 + 12.f, 48.f + 26.f * i, Fonte, E);
			}
		}
	}

	// ---- barra de construcao (7 maquinas, a escolhida no meio)
	const float Y = Canvas->ClipY - 96.f;
	const float LarguraCaixa = 190.f;
	float X = (Canvas->ClipX - 7.f * (LarguraCaixa + 8.f)) * 0.5f;
	for (int32 k = -3; k <= 3; k++)
	{
		const int32 T = (PC->Ferramenta + k + FerM::NUM) % FerM::NUM;
		const bool Escolhida = k == 0;
		const bool Travada = N < GFerMaq[T].Nivel;
		DrawRect(Escolhida ? Laranja : Fundo, X, Y, LarguraCaixa, 76.f);
		const FLinearColor CorTexto = Escolhida ? FLinearColor::Black : (Travada ? Hex(TEXT("6B7785")) : FLinearColor::White);
		DrawText(NomeMaq(T), CorTexto, X + 8.f, Y + 6.f, Fonte, Escolhida ? E : 1.f);
		const FString Sub = Travada ? FString::Printf(TEXT("n\u00edvel %d"), GFerMaq[T].Nivel) : FString::Printf(TEXT("$ %d"), M->Preco(T));
		DrawText(Sub, Escolhida ? FLinearColor::Black : Cinza, X + 8.f, Y + 40.f, Fonte, Escolhida ? E : 1.f);
		X += LarguraCaixa + 8.f;
	}

	// receitas da maquina escolhida
	const FString Rec = ReceitasTexto(PC->Ferramenta);
	if (!Rec.IsEmpty())
	{
		DrawText(Rec, Laranja, 20.f, Y - 64.f, Fonte, 1.f);
	}

	static const TCHAR* Direcoes[4] = { TEXT("cima"), TEXT("direita"), TEXT("baixo"), TEXT("esquerda") };
	DrawText(FString::Printf(TEXT("Q / E: trocar m\u00e1quina   |   1 Mineradora  2 Esteira  3 Forno  4 Mercado  5 Gerador   |   R: dire\u00e7\u00e3o (%s)   |   Bot\u00e3o direito: remover   |   WASD: mover   |   Roda ou Z/X: zoom"), Direcoes[PC->Dir & 3]), Cinza, 20.f, Y - 34.f, Fonte, 1.f);

	// ---- aviso no meio da tela
	if (M->AvisoTempo > 0.f && !M->Aviso.IsEmpty())
	{
		float W = 0.f, H = 0.f;
		GetTextSize(M->Aviso, W, H, FonteGrande, 1.f);
		DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.72f), (Canvas->ClipX - W) * 0.5f - 16.f, Canvas->ClipY * 0.32f - 8.f, W + 32.f, H + 16.f);
		DrawText(M->Aviso, Hex(TEXT("FFD18A")), (Canvas->ClipX - W) * 0.5f, Canvas->ClipY * 0.32f, FonteGrande, 1.f);
	}
}

// =====================================================================
// MODO DE JOGO
// =====================================================================

AFerGameMode::AFerGameMode()
{
	DefaultPawnClass = AFerCamera::StaticClass();
	PlayerControllerClass = AFerControle::StaticClass();
	HUDClass = AFerHUD::StaticClass();
}

void AFerGameMode::BeginPlay()
{
	Super::BeginPlay();

	// cria o mundo da fabrica, se ainda nao existir no mapa
	bool Existe = false;
	for (TActorIterator<AFerMundo> It(GetWorld()); It; ++It)
	{
		Existe = true;
		break;
	}
	if (!Existe)
	{
		GetWorld()->SpawnActor<AFerMundo>(AFerMundo::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	}
}
