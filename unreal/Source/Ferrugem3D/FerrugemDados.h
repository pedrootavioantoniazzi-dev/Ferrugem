// Ferrugem 3D - tabelas do jogo (maquinas, itens e receitas)
// Gerado automaticamente a partir do Ferrugem 2D (index.html), para os numeros serem os mesmos.
// Coloque este arquivo em Source/Ferrugem3D/, junto com FerrugemJogo.h e FerrugemJogo.cpp.

#pragma once

#include "CoreMinimal.h"

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
