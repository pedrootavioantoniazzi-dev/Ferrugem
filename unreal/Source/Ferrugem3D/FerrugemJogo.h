// Ferrugem 3D - (Nexvora)
// As 34 maquinas, 36 itens e receitas do Ferrugem 2D, numa grade 3D.
// Feito para Unreal Engine 5.1 (projeto C++). Coloque em Source/Ferrugem3D/ junto com
// FerrugemJogo.cpp e FerrugemDados.h.

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
class UTextRenderComponent;
class AStaticMeshActor;

// Um item andando em cima de uma esteira
USTRUCT()
struct FFerItemNaEsteira
{
	GENERATED_BODY()

	int32 Item = -1;

	// 0 = comeco da esteira, 1 = fim
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

	void Iniciar(class AFerMundo* NovoMundo, int32 NovoTipo, FIntPoint NovaPos, int32 NovaDir);
	void Girar(int32 NovaDir);

	bool Aceita(int32 Item) const;
	void Receber(int32 Item);

	bool Gerando() const;            // esta gerando energia agora?
	FString EstadoTexto() const;
	FString ConteudoTexto() const;

	int32 Tipo = 0;                  // indice em GFerMaq (FerM::...)
	FIntPoint Pos = FIntPoint(0, 0);
	int32 Dir = 0;                   // 0 = +X, 1 = +Y, 2 = -X, 3 = -Y

	TMap<int32, int32> Estoque;      // itens guardados dentro (entrada)
	TArray<int32> Saida;             // itens prontos para sair
	int32 Receita = -1;              // receita sendo feita (indice em GFerReceitas)
	float Progresso = 0.f;
	float Queima = 0.f;              // gerador: tempo de carvao queimando
	int32 Minerio = -1;              // mineradora/bomba: item que ela tira do chao
	bool Quer = false;               // quer energia neste momento
	uint8 Estado = 0;                // 0 funcionando, 1 sem material, 2 sem energia, 3 saida cheia

	UPROPERTY()
	class AFerMundo* Mundo = nullptr;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Raiz;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Corpo;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Detalhe;

	UPROPERTY(VisibleAnywhere)
	UTextRenderComponent* Rotulo;

	UPROPERTY()
	UMaterialInstanceDynamic* MatDetalhe = nullptr;

	UPROPERTY()
	TArray<FFerItemNaEsteira> Itens;

private:
	FLinearColor CorLuz = FLinearColor::White;
	int32 LuzVista = -1;

	void TickEsteira(float Dt);
	void TickReceita(float Dt);
	void TickMineradora(float Dt);
	void Empurrar();
	void EmpurrarEstoque();
	void Luz(bool Acesa);
	int32 Total() const;
	UStaticMeshComponent* CriarVisualItem(int32 Item);
};

// ===================== MUNDO (grade, minerio, dinheiro, energia, nivel) =====================
UCLASS()
class AFerMundo : public AActor
{
	GENERATED_BODY()

public:
	AFerMundo();
	virtual void BeginPlay() override;
	virtual void Tick(float Dt) override;

	static constexpr float Tam = 100.f;  // tamanho de um quadrado (1 metro)
	static constexpr int32 Lado = 60;    // a grade tem 60 x 60 quadrados

	FVector Centro(FIntPoint P) const;
	FIntPoint Tile(const FVector& V) const;
	bool Dentro(FIntPoint P) const;
	int32 MinerioEm(FIntPoint P) const;  // 0 = nada, 1..6 = tipo de minerio
	AFerMaquina* Em(FIntPoint P) const;

	int32 Nivel() const;
	int64 XPDoNivel(int32 N) const;
	float VelEsteira() const;
	int32 Contar(int32 Tipo) const;
	int32 Preco(int32 Tipo) const;

	// "" se pode construir, ou o motivo de nao poder
	FString Motivo(int32 Tipo, FIntPoint P) const;
	FString Construir(int32 Tipo, FIntPoint P, int32 Dir);
	void Remover(FIntPoint P);
	void Vender(int32 Item);

	static UStaticMesh* Malha(const TCHAR* NomeMalha); // "Cube", "Sphere", "Cylinder", "Cone", "Plane"
	static UMaterialInterface* MaterialBase();
	static UMaterialInstanceDynamic* MaterialCor(UObject* Dono, FLinearColor C);
	UMaterialInstanceDynamic* MaterialDoItem(int32 Item);

	void Avisar(const FString& Texto, float Tempo = 3.f);

	double Dinheiro = 650.0;
	double XP = 0.0;
	int32 Vendidos = 0;
	float Sup = 0.f;   // energia gerada (kW)
	float Dem = 0.f;   // energia pedida (kW)
	float Sat = 1.f;   // quanto da energia pedida esta sendo atendida (0 a 1)
	FString Aviso;
	float AvisoTempo = 0.f;

	TMap<FIntPoint, int32> Minerio;
	TMap<FIntPoint, TWeakObjectPtr<AFerMaquina>> Maquinas;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Raiz;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Chao;

	UPROPERTY(VisibleAnywhere)
	UInstancedStaticMeshComponent* Linhas;

	UPROPERTY()
	TArray<UInstancedStaticMeshComponent*> Cristais;

	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> MatItens;

private:
	int32 NivelVisto = 1;
	void CriarJazida(int32 Tipo, FIntPoint C, int32 Raio);
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

	int32 Ferramenta = 1;   // maquina escolhida (FerM::...)
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
	void Teclas();
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
