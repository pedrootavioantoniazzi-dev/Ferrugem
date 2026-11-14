// Ferrugem 3D - prototipo (Nexvora)
// Mineradora -> Esteira -> Forno -> Mercado, numa grade, com formas 3D basicas da Unreal.
// Feito para Unreal Engine 5.1 (projeto C++). Coloque este arquivo em Source/Ferrugem3D/.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/HUD.h"
#include "FerrugemJogo.generated.h"

class UStaticMesh;
class UStaticMeshComponent;
class UInstancedStaticMeshComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class USpringArmComponent;
class UCameraComponent;
class AStaticMeshActor;

// Tipos de maquina
UENUM()
enum class EFerTipo : uint8
{
	Nenhum,
	Mineradora,
	Esteira,
	Forno,
	Mercado
};

// Tipos de item
UENUM()
enum class EFerItem : uint8
{
	Nenhum,
	MinerioFerro,
	LingoteFerro
};

// Um item andando em cima de uma esteira
USTRUCT()
struct FFerItemNaEsteira
{
	GENERATED_BODY()

	EFerItem Item = EFerItem::Nenhum;

	// 0 = comeco da esteira, 1 = fim da esteira
	float T = 0.f;

	UPROPERTY()
	UStaticMeshComponent* Visual = nullptr;
};

// ===================== MAQUINA =====================
UCLASS()
class AFerMaquina : public AActor
{
	GENERATED_BODY()

public:
	AFerMaquina();
	virtual void Tick(float Dt) override;

	void Iniciar(class AFerMundo* NovoMundo, EFerTipo NovoTipo, FIntPoint NovaPos, int32 NovaDir);
	void Girar(int32 NovaDir);

	// A maquina aceita receber este item agora?
	bool Aceita(EFerItem Item) const;
	void Receber(EFerItem Item);

	EFerTipo Tipo = EFerTipo::Nenhum;
	FIntPoint Pos = FIntPoint(0, 0);
	int32 Dir = 0; // 0 = +X, 1 = +Y, 2 = -X, 3 = -Y

	UPROPERTY()
	class AFerMundo* Mundo = nullptr;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Raiz;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Corpo;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Detalhe;

	UPROPERTY()
	UMaterialInstanceDynamic* MatDetalhe = nullptr;

	// Itens em cima (so a esteira usa)
	UPROPERTY()
	TArray<FFerItemNaEsteira> Itens;

	int32 Entrada = 0;       // itens esperando para entrar (forno)
	TArray<EFerItem> Saida;  // itens prontos para sair
	float Progresso = 0.f;
	bool Trabalhando = false;

private:
	void Empurrar();
	UStaticMeshComponent* CriarVisualItem(EFerItem Item);
};

// ===================== MUNDO (grade, minerio, dinheiro) =====================
UCLASS()
class AFerMundo : public AActor
{
	GENERATED_BODY()

public:
	AFerMundo();
	virtual void BeginPlay() override;
	virtual void Tick(float Dt) override;

	static constexpr float Tam = 100.f;  // tamanho de um quadrado da grade (1 metro)
	static constexpr int32 Lado = 40;    // a grade tem 40 x 40 quadrados

	FVector Centro(FIntPoint P) const;
	FIntPoint Tile(const FVector& V) const;
	bool Dentro(FIntPoint P) const;
	bool TemMinerio(FIntPoint P) const { return Minerio.Contains(P); }
	AFerMaquina* Em(FIntPoint P) const;

	// Retorna "" se deu certo, ou o motivo de nao ter dado
	FString Construir(EFerTipo Tipo, FIntPoint P, int32 Dir);
	void Remover(FIntPoint P);

	static int32 Preco(EFerTipo Tipo);
	static FString Nome(EFerTipo Tipo);
	static float Valor(EFerItem Item);
	static FLinearColor Cor(EFerTipo Tipo);
	static FLinearColor CorItem(EFerItem Item);

	static UStaticMesh* Malha(const TCHAR* NomeMalha); // "Cube", "Sphere", "Cylinder", "Cone", "Plane"
	static UMaterialInterface* MaterialBase();
	static UMaterialInstanceDynamic* MaterialCor(UObject* Dono, FLinearColor C);

	UMaterialInstanceDynamic* MaterialDoItem(EFerItem Item);

	void Avisar(const FString& Texto);

	double Dinheiro = 300.0;
	int32 Vendidos = 0;
	FString Aviso;
	float AvisoTempo = 0.f;

	TSet<FIntPoint> Minerio;
	TMap<FIntPoint, TWeakObjectPtr<AFerMaquina>> Maquinas;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Raiz;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Chao;

	UPROPERTY(VisibleAnywhere)
	UInstancedStaticMeshComponent* Linhas;

	UPROPERTY(VisibleAnywhere)
	UInstancedStaticMeshComponent* Cristais;

	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> MatItens;

private:
	void CriarJazida(FIntPoint C, int32 Raio);
};

// ===================== CAMERA (vista de cima) =====================
UCLASS()
class AFerCamera : public APawn
{
	GENERATED_BODY()

public:
	AFerCamera();

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Raiz;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* Braco;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;
};

// ===================== CONTROLE (mouse e teclado) =====================
UCLASS()
class AFerControle : public APlayerController
{
	GENERATED_BODY()

public:
	AFerControle();
	virtual void BeginPlay() override;
	virtual void PlayerTick(float Dt) override;

	EFerTipo Ferramenta = EFerTipo::Mineradora;
	int32 Dir = 0;
	FIntPoint Mira = FIntPoint(0, 0);
	bool MiraValida = false;

	UPROPERTY()
	AFerMundo* Mundo = nullptr;

private:
	UPROPERTY()
	AStaticMeshActor* Fantasma = nullptr;

	UPROPERTY()
	AStaticMeshActor* Seta = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* MatFantasma = nullptr;

	FIntPoint UltimoArrasto = FIntPoint(-999, -999);
	bool Posicionou = false;

	AStaticMeshActor* CriarPeca(const TCHAR* NomeMalha, FVector Escala, UMaterialInstanceDynamic* Mat);
	void MoverCamera(float Dt);
	void AtualizarMira();
	void Clicar();
};

// ===================== HUD (textos na tela) =====================
UCLASS()
class AFerHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
};

// ===================== MODO DE JOGO =====================
UCLASS()
class AFerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFerGameMode();
	virtual void BeginPlay() override;
};
