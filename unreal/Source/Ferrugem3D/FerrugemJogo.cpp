// Ferrugem 3D - prototipo (Nexvora)
// Coloque este arquivo em Source/Ferrugem3D/, junto com FerrugemJogo.h.

#include "FerrugemJogo.h"

#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/Canvas.h"
#include "EngineUtils.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Direcao -> passo na grade (0 = +X, 1 = +Y, 2 = -X, 3 = -Y)
static FIntPoint DirDelta(int32 D)
{
	static const int32 DX[4] = { 1, 0, -1, 0 };
	static const int32 DY[4] = { 0, 1, 0, -1 };
	return FIntPoint(DX[D & 3], DY[D & 3]);
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

	Cristais = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Cristais"));
	Cristais->SetupAttachment(Raiz);
	Cristais->SetMobility(EComponentMobility::Movable);
	Cristais->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AFerMundo::BeginPlay()
{
	Super::BeginPlay();

	const float Total = Lado * Tam;

	// Chao escuro
	Chao->SetStaticMesh(Malha(TEXT("Plane")));
	Chao->SetMaterial(0, MaterialCor(this, FLinearColor(0.05f, 0.07f, 0.09f)));
	Chao->SetRelativeLocation(FVector(Total * 0.5f, Total * 0.5f, 0.5f));
	Chao->SetRelativeScale3D(FVector((float)Lado, (float)Lado, 1.f));

	// Linhas da grade
	Linhas->SetStaticMesh(Malha(TEXT("Cube")));
	Linhas->SetMaterial(0, MaterialCor(this, FLinearColor(0.13f, 0.16f, 0.2f)));
	for (int32 i = 0; i <= Lado; i++)
	{
		Linhas->AddInstance(FTransform(FRotator::ZeroRotator, FVector(i * Tam, Total * 0.5f, 1.5f), FVector(0.03f, Total / 100.f, 0.01f)));
		Linhas->AddInstance(FTransform(FRotator::ZeroRotator, FVector(Total * 0.5f, i * Tam, 1.5f), FVector(Total / 100.f, 0.03f, 0.01f)));
	}

	// Cristais de minerio de ferro (laranja, como no 2D)
	Cristais->SetStaticMesh(Malha(TEXT("Cone")));
	Cristais->SetMaterial(0, MaterialCor(this, FLinearColor(0.95f, 0.5f, 0.15f)));
	CriarJazida(FIntPoint(12, 12), 4);
	CriarJazida(FIntPoint(26, 10), 3);
	CriarJazida(FIntPoint(9, 27), 3);

	// Cores dos itens (uma por tipo)
	MatItens.SetNum(3);
	MatItens[(int32)EFerItem::MinerioFerro] = MaterialCor(this, CorItem(EFerItem::MinerioFerro));
	MatItens[(int32)EFerItem::LingoteFerro] = MaterialCor(this, CorItem(EFerItem::LingoteFerro));
}

void AFerMundo::Tick(float Dt)
{
	Super::Tick(Dt);
	if (AvisoTempo > 0.f)
	{
		AvisoTempo -= Dt;
	}
}

void AFerMundo::CriarJazida(FIntPoint C, int32 Raio)
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
			Minerio.Add(P);
			const FVector Lugar = Centro(P) + FVector(FMath::FRandRange(-15.f, 15.f), FMath::FRandRange(-15.f, 15.f), 25.f);
			Cristais->AddInstance(FTransform(FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f), Lugar, FVector(0.35f, 0.35f, 0.5f)));
		}
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

AFerMaquina* AFerMundo::Em(FIntPoint P) const
{
	const TWeakObjectPtr<AFerMaquina>* W = Maquinas.Find(P);
	return (W && W->IsValid()) ? W->Get() : nullptr;
}

FString AFerMundo::Construir(EFerTipo Tipo, FIntPoint P, int32 Dir)
{
	if (!Dentro(P))
	{
		return TEXT("Fora do mapa");
	}
	if (Em(P))
	{
		return TEXT("J\u00e1 tem uma m\u00e1quina aqui");
	}
	if (Tipo == EFerTipo::Mineradora && !TemMinerio(P))
	{
		return TEXT("A mineradora precisa ficar em cima do min\u00e9rio");
	}
	const int32 Custo = Preco(Tipo);
	if (Dinheiro < Custo)
	{
		return TEXT("Dinheiro insuficiente");
	}

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
	Dinheiro += Preco(M->Tipo) / 2;
	Maquinas.Remove(P);
	M->Destroy();
}

void AFerMundo::Avisar(const FString& Texto)
{
	Aviso = Texto;
	AvisoTempo = 2.5f;
}

int32 AFerMundo::Preco(EFerTipo Tipo)
{
	switch (Tipo)
	{
	case EFerTipo::Mineradora: return 50;
	case EFerTipo::Esteira:    return 5;
	case EFerTipo::Forno:      return 80;
	case EFerTipo::Mercado:    return 40;
	default:                   return 0;
	}
}

FString AFerMundo::Nome(EFerTipo Tipo)
{
	switch (Tipo)
	{
	case EFerTipo::Mineradora: return TEXT("Mineradora");
	case EFerTipo::Esteira:    return TEXT("Esteira");
	case EFerTipo::Forno:      return TEXT("Forno");
	case EFerTipo::Mercado:    return TEXT("Mercado");
	default:                   return TEXT("?");
	}
}

float AFerMundo::Valor(EFerItem Item)
{
	switch (Item)
	{
	case EFerItem::MinerioFerro: return 2.f;
	case EFerItem::LingoteFerro: return 12.f;
	default:                     return 0.f;
	}
}

FLinearColor AFerMundo::Cor(EFerTipo Tipo)
{
	switch (Tipo)
	{
	case EFerTipo::Mineradora: return FLinearColor(0.79f, 0.47f, 0.17f);
	case EFerTipo::Esteira:    return FLinearColor(0.2f, 0.23f, 0.27f);
	case EFerTipo::Forno:      return FLinearColor(0.72f, 0.27f, 0.17f);
	case EFerTipo::Mercado:    return FLinearColor(0.18f, 0.62f, 0.42f);
	default:                   return FLinearColor::White;
	}
}

FLinearColor AFerMundo::CorItem(EFerItem Item)
{
	switch (Item)
	{
	case EFerItem::MinerioFerro: return FLinearColor(0.8f, 0.42f, 0.2f);
	case EFerItem::LingoteFerro: return FLinearColor(0.92f, 0.78f, 0.65f);
	default:                     return FLinearColor::White;
	}
}

UStaticMesh* AFerMundo::Malha(const TCHAR* NomeMalha)
{
	const FString Caminho = FString::Printf(TEXT("/Engine/BasicShapes/%s.%s"), NomeMalha, NomeMalha);
	return LoadObject<UStaticMesh>(nullptr, *Caminho);
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

UMaterialInstanceDynamic* AFerMundo::MaterialDoItem(EFerItem Item)
{
	const int32 i = (int32)Item;
	return MatItens.IsValidIndex(i) ? MatItens[i] : nullptr;
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
}

void AFerMaquina::Iniciar(AFerMundo* NovoMundo, EFerTipo NovoTipo, FIntPoint NovaPos, int32 NovaDir)
{
	Mundo = NovoMundo;
	Tipo = NovoTipo;
	Pos = NovaPos;
	Girar(NovaDir);

	Corpo->SetMaterial(0, AFerMundo::MaterialCor(this, AFerMundo::Cor(Tipo)));

	switch (Tipo)
	{
	case EFerTipo::Mineradora:
		// caixa laranja com uma broca girando em cima
		Corpo->SetStaticMesh(AFerMundo::Malha(TEXT("Cube")));
		Corpo->SetRelativeScale3D(FVector(0.8f, 0.8f, 0.6f));
		Corpo->SetRelativeLocation(FVector(0.f, 0.f, 30.f));
		Detalhe->SetStaticMesh(AFerMundo::Malha(TEXT("Cylinder")));
		Detalhe->SetRelativeScale3D(FVector(0.3f, 0.3f, 0.7f));
		Detalhe->SetRelativeLocation(FVector(0.f, 0.f, 75.f));
		MatDetalhe = AFerMundo::MaterialCor(this, FLinearColor(0.6f, 0.62f, 0.65f));
		break;

	case EFerTipo::Esteira:
		// faixa baixa e escura com uma seta amarela mostrando a direcao
		Corpo->SetStaticMesh(AFerMundo::Malha(TEXT("Cube")));
		Corpo->SetRelativeScale3D(FVector(1.f, 0.8f, 0.1f));
		Corpo->SetRelativeLocation(FVector(0.f, 0.f, 5.f));
		Detalhe->SetStaticMesh(AFerMundo::Malha(TEXT("Cone")));
		Detalhe->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.2f));
		Detalhe->SetRelativeLocation(FVector(30.f, 0.f, 15.f));
		Detalhe->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
		MatDetalhe = AFerMundo::MaterialCor(this, FLinearColor(0.89f, 0.66f, 0.31f));
		break;

	case EFerTipo::Forno:
		// bloco vermelho com uma "boca" que acende quando esta derretendo
		Corpo->SetStaticMesh(AFerMundo::Malha(TEXT("Cube")));
		Corpo->SetRelativeScale3D(FVector(0.85f, 0.85f, 0.9f));
		Corpo->SetRelativeLocation(FVector(0.f, 0.f, 45.f));
		Detalhe->SetStaticMesh(AFerMundo::Malha(TEXT("Sphere")));
		Detalhe->SetRelativeScale3D(FVector(0.35f));
		Detalhe->SetRelativeLocation(FVector(40.f, 0.f, 35.f));
		MatDetalhe = AFerMundo::MaterialCor(this, FLinearColor(0.08f, 0.03f, 0.02f));
		break;

	case EFerTipo::Mercado:
		// caixa verde com uma moeda dourada girando
		Corpo->SetStaticMesh(AFerMundo::Malha(TEXT("Cube")));
		Corpo->SetRelativeScale3D(FVector(0.85f, 0.85f, 0.5f));
		Corpo->SetRelativeLocation(FVector(0.f, 0.f, 25.f));
		Detalhe->SetStaticMesh(AFerMundo::Malha(TEXT("Cylinder")));
		Detalhe->SetRelativeScale3D(FVector(0.45f, 0.45f, 0.06f));
		Detalhe->SetRelativeLocation(FVector(0.f, 0.f, 80.f));
		Detalhe->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
		MatDetalhe = AFerMundo::MaterialCor(this, FLinearColor(1.f, 0.8f, 0.2f));
		break;

	default:
		break;
	}

	Detalhe->SetMaterial(0, MatDetalhe);
}

void AFerMaquina::Girar(int32 NovaDir)
{
	Dir = NovaDir & 3;
	SetActorRotation(FRotator(0.f, Dir * 90.f, 0.f));
}

bool AFerMaquina::Aceita(EFerItem Item) const
{
	if (Item == EFerItem::Nenhum)
	{
		return false;
	}
	switch (Tipo)
	{
	case EFerTipo::Esteira:
		return Itens.Num() < 4 && (Itens.Num() == 0 || Itens.Last().T >= 0.34f);
	case EFerTipo::Forno:
		return Item == EFerItem::MinerioFerro && Entrada < 10;
	case EFerTipo::Mercado:
		return true;
	default:
		return false;
	}
}

void AFerMaquina::Receber(EFerItem Item)
{
	switch (Tipo)
	{
	case EFerTipo::Esteira:
	{
		FFerItemNaEsteira Novo;
		Novo.Item = Item;
		Novo.T = 0.f;
		Novo.Visual = CriarVisualItem(Item);
		Itens.Add(Novo);
		break;
	}
	case EFerTipo::Forno:
		Entrada++;
		break;
	case EFerTipo::Mercado:
		if (Mundo)
		{
			Mundo->Dinheiro += AFerMundo::Valor(Item);
			Mundo->Vendidos++;
		}
		break;
	default:
		break;
	}
}

UStaticMeshComponent* AFerMaquina::CriarVisualItem(EFerItem Item)
{
	UStaticMeshComponent* V = NewObject<UStaticMeshComponent>(this);
	V->SetMobility(EComponentMobility::Movable);
	V->SetStaticMesh(AFerMundo::Malha(Item == EFerItem::MinerioFerro ? TEXT("Sphere") : TEXT("Cube")));
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

void AFerMaquina::Empurrar()
{
	if (Saida.Num() == 0 || !Mundo)
	{
		return;
	}
	AFerMaquina* Frente = Mundo->Em(Pos + DirDelta(Dir));
	if (Frente && Frente != this && Frente->Aceita(Saida[0]))
	{
		const EFerItem Item = Saida[0];
		Saida.RemoveAt(0);
		Frente->Receber(Item);
	}
}

void AFerMaquina::Tick(float Dt)
{
	Super::Tick(Dt);
	if (!Mundo)
	{
		return;
	}

	switch (Tipo)
	{
	case EFerTipo::Mineradora:
		// tira 1 minerio a cada 1,5 segundo (se a saida nao estiver cheia)
		Trabalhando = Saida.Num() < 3;
		if (Trabalhando)
		{
			Progresso += Dt;
			if (Progresso >= 1.5f)
			{
				Progresso = 0.f;
				Saida.Add(EFerItem::MinerioFerro);
			}
			Detalhe->AddLocalRotation(FRotator(0.f, 360.f * Dt, 0.f));
		}
		Empurrar();
		break;

	case EFerTipo::Forno:
		// derrete 1 minerio em 2 segundos e vira 1 lingote
		if (!Trabalhando && Entrada > 0 && Saida.Num() < 3)
		{
			Entrada--;
			Trabalhando = true;
			Progresso = 0.f;
			if (MatDetalhe)
			{
				MatDetalhe->SetVectorParameterValue(TEXT("Color"), FLinearColor(1.f, 0.45f, 0.05f));
			}
		}
		if (Trabalhando)
		{
			Progresso += Dt;
			if (Progresso >= 2.f)
			{
				Trabalhando = false;
				Saida.Add(EFerItem::LingoteFerro);
				if (MatDetalhe)
				{
					MatDetalhe->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.08f, 0.03f, 0.02f));
				}
			}
		}
		Empurrar();
		break;

	case EFerTipo::Mercado:
		Detalhe->AddWorldRotation(FRotator(0.f, 90.f * Dt, 0.f));
		break;

	case EFerTipo::Esteira:
	{
		// os itens andam ate o fim, sem encostar um no outro
		for (int32 i = 0; i < Itens.Num(); i++)
		{
			const float Limite = (i == 0) ? 1.f : (Itens[i - 1].T - 0.34f);
			Itens[i].T = FMath::Max(Itens[i].T, FMath::Min(Itens[i].T + Dt * 1.2f, Limite));
			if (Itens[i].Visual)
			{
				Itens[i].Visual->SetRelativeLocation(FVector(-50.f + 100.f * Itens[i].T, 0.f, 20.f));
			}
		}
		// o primeiro item chegou no fim: passa para a maquina da frente
		if (Itens.Num() > 0 && Itens[0].T >= 1.f)
		{
			AFerMaquina* Frente = Mundo->Em(Pos + DirDelta(Dir));
			const bool Contramao = Frente && Frente->Tipo == EFerTipo::Esteira && (Frente->Pos + DirDelta(Frente->Dir)) == Pos;
			if (Frente && Frente != this && !Contramao && Frente->Aceita(Itens[0].Item))
			{
				const EFerItem Item = Itens[0].Item;
				if (Itens[0].Visual)
				{
					Itens[0].Visual->DestroyComponent();
				}
				Itens.RemoveAt(0);
				Frente->Receber(Item);
			}
		}
		break;
	}

	default:
		break;
	}
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
	Braco->TargetArmLength = 2200.f;
	Braco->SetRelativeRotation(FRotator(-55.f, 0.f, 0.f));
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

	// "fantasma": mostra onde a maquina vai ser construida (verde = pode, vermelho = nao pode)
	MatFantasma = AFerMundo::MaterialCor(this, FLinearColor(0.1f, 0.8f, 0.2f));
	Fantasma = CriarPeca(TEXT("Cube"), FVector(0.95f, 0.95f, 0.08f), MatFantasma);
	// seta branca: mostra para onde a maquina vai mandar os itens
	Seta = CriarPeca(TEXT("Cone"), FVector(0.25f), AFerMundo::MaterialCor(this, FLinearColor(0.95f, 0.95f, 0.95f)));
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

	// coloca a camera em cima da primeira jazida de minerio
	if (!Posicionou && GetPawn())
	{
		GetPawn()->SetActorLocation(Mundo->Centro(FIntPoint(14, 13)));
		Posicionou = true;
	}

	MoverCamera(Dt);

	// teclas 1 a 4 escolhem a maquina, R gira
	if (WasInputKeyJustPressed(EKeys::One))   Ferramenta = EFerTipo::Mineradora;
	if (WasInputKeyJustPressed(EKeys::Two))   Ferramenta = EFerTipo::Esteira;
	if (WasInputKeyJustPressed(EKeys::Three)) Ferramenta = EFerTipo::Forno;
	if (WasInputKeyJustPressed(EKeys::Four))  Ferramenta = EFerTipo::Mercado;
	if (WasInputKeyJustPressed(EKeys::R))     Dir = (Dir + 1) % 4;

	AtualizarMira();
	Clicar();
}

void AFerControle::MoverCamera(float Dt)
{
	AFerCamera* Cam = Cast<AFerCamera>(GetPawn());
	if (!Cam)
	{
		return;
	}

	// WASD: a camera olha para +X, entao W = +X e D = +Y
	FVector Mov(0.f);
	if (IsInputKeyDown(EKeys::W)) Mov.X += 1.f;
	if (IsInputKeyDown(EKeys::S)) Mov.X -= 1.f;
	if (IsInputKeyDown(EKeys::D)) Mov.Y += 1.f;
	if (IsInputKeyDown(EKeys::A)) Mov.Y -= 1.f;
	const float Velocidade = Cam->Braco->TargetArmLength * 0.9f;
	Cam->AddActorWorldOffset(Mov.GetSafeNormal() * Velocidade * Dt);

	// zoom: roda do mouse, ou Z (aproxima) e X (afasta)
	float Zoom = GetInputAnalogKeyState(EKeys::MouseWheelAxis) * 250.f;
	if (WasInputKeyJustPressed(EKeys::MouseScrollUp))   Zoom += 250.f;
	if (WasInputKeyJustPressed(EKeys::MouseScrollDown)) Zoom -= 250.f;
	if (IsInputKeyDown(EKeys::Z)) Zoom += 1800.f * Dt;
	if (IsInputKeyDown(EKeys::X)) Zoom -= 1800.f * Dt;
	Cam->Braco->TargetArmLength = FMath::Clamp(Cam->Braco->TargetArmLength - Zoom, 500.f, 5000.f);
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
			const bool Pode = !Mundo->Em(Mira)
				&& (Ferramenta != EFerTipo::Mineradora || Mundo->TemMinerio(Mira))
				&& Mundo->Dinheiro >= AFerMundo::Preco(Ferramenta);
			if (MatFantasma)
			{
				MatFantasma->SetVectorParameterValue(TEXT("Color"), Pode ? FLinearColor(0.1f, 0.8f, 0.2f) : FLinearColor(0.9f, 0.15f, 0.1f));
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
	if (Ferramenta != EFerTipo::Esteira)
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
		if (Anterior && Anterior->Tipo == EFerTipo::Esteira)
		{
			Anterior->Girar(Dir);
		}
	}
	if (!Mundo->Em(Mira))
	{
		const FString Erro = Mundo->Construir(EFerTipo::Esteira, Mira, Dir);
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
	const FLinearColor Laranja(1.f, 0.62f, 0.11f);
	const FLinearColor Cinza(0.7f, 0.75f, 0.8f);

	// dinheiro e vendas (canto de cima)
	DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.6f), 10.f, 10.f, 360.f, 76.f);
	DrawText(FString::Printf(TEXT("$ %s"), *FText::AsNumber(FMath::FloorToInt(M->Dinheiro)).ToString()), Laranja, 22.f, 14.f, FonteGrande, 1.f);
	DrawText(FString::Printf(TEXT("Itens vendidos: %d"), M->Vendidos), FLinearColor::White, 22.f, 54.f, Fonte, 1.f);

	// barra de construcao (embaixo)
	const float Y = Canvas->ClipY - 74.f;
	float X = 20.f;
	for (int32 i = 1; i <= 4; i++)
	{
		const EFerTipo T = static_cast<EFerTipo>(i);
		const bool Escolhida = PC->Ferramenta == T;
		DrawRect(Escolhida ? FLinearColor(1.f, 0.62f, 0.11f, 0.95f) : FLinearColor(0.f, 0.f, 0.f, 0.6f), X, Y, 200.f, 58.f);
		DrawText(FString::Printf(TEXT("[%d] %s"), i, *AFerMundo::Nome(T)), Escolhida ? FLinearColor::Black : FLinearColor::White, X + 10.f, Y + 6.f, Fonte, 1.f);
		DrawText(FString::Printf(TEXT("$ %d"), AFerMundo::Preco(T)), Escolhida ? FLinearColor::Black : Cinza, X + 10.f, Y + 30.f, Fonte, 1.f);
		X += 210.f;
	}
	static const TCHAR* Direcoes[4] = { TEXT("cima"), TEXT("direita"), TEXT("baixo"), TEXT("esquerda") };
	DrawText(FString::Printf(TEXT("Dire\u00e7\u00e3o (R): %s"), Direcoes[PC->Dir & 3]), FLinearColor::White, X + 10.f, Y + 18.f, Fonte, 1.f);

	DrawText(TEXT("Clique: construir  |  Arraste: esteiras  |  Bot\u00e3o direito: remover  |  WASD: mover  |  Roda ou Z/X: zoom"), Cinza, 20.f, Y - 30.f, Fonte, 1.f);

	// aviso no meio da tela
	if (M->AvisoTempo > 0.f && !M->Aviso.IsEmpty())
	{
		float W = 0.f, H = 0.f;
		GetTextSize(M->Aviso, W, H, FonteGrande, 1.f);
		DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.7f), (Canvas->ClipX - W) * 0.5f - 16.f, Canvas->ClipY * 0.4f - 8.f, W + 32.f, H + 16.f);
		DrawText(M->Aviso, FLinearColor(1.f, 0.55f, 0.5f), (Canvas->ClipX - W) * 0.5f, Canvas->ClipY * 0.4f, FonteGrande, 1.f);
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
