#include "stdafx.h"
#include "ConvModel.h"
#include "Converter.h"

void ConvModel::Initialize()
{
	//Airplane();
	//Tower();
	//Kachujin();
	Tank();
	//Weapons();
}

void ConvModel::Airplane()
{
	Converter* conv = new Converter();
	conv->ReadFile(L"B787/Airplane.fbx");
	conv->ExportMaterial(L"B787/Airplane", false);
	conv->ExportMesh(L"B787/Airplane");

	SafeDelete(conv);
}

void ConvModel::Tower()
{
	Converter* conv = new Converter();
	conv->ReadFile(L"Tower/Tower.fbx");
	conv->ExportMaterial(L"Tower/Tower", false);
	conv->ExportMesh(L"Tower/Tower");

	SafeDelete(conv);
}

void ConvModel::Tank()
{
	//Converter* conv = new Converter();
	//conv->ReadFile(L"Tank/Tank.fbx");
	//conv->ExportMaterial(L"Tank/Tank", false);
	//conv->ExportMesh(L"Tank/Tank");

	Converter* conv = new Converter();
	conv->ReadFile(L"Weapon/BOW AND ARROW.fbx");
	conv->ExportMaterial(L"Weapon/Bow", false);
	conv->ExportMesh(L"Weapon/Bow");

	SafeDelete(conv);
	//conv = new Converter();
	//conv->ReadFile(L"Weapon/LongArrow.obj");
	//conv->ExportMaterial(L"Weapon/LongArrow", true);
	//conv->ExportMesh(L"Weapon/LongArrow");

	//SafeDelete(conv);
}

void ConvModel::Kachujin()
{
	Converter* conv = new Converter();
	conv->ReadFile(L"Archer/akai_e_espiritu.fbx");
	conv->ExportMaterial(L"Archer/Mesh", false);
	conv->ExportMesh(L"Archer/Mesh");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Archer/Idle.fbx");
	conv->ExportAnimClip(0, L"Archer/Idle");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Archer/Standing Run Forward.fbx");
	conv->ExportAnimClip(0, L"Archer/Running");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Archer/Standing Draw Arrow.fbx");
	conv->ExportAnimClip(0, L"Archer/Draw");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Archer/Standing Aim Recoil.fbx");
	conv->ExportAnimClip(0, L"Archer/Shoot");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Archer/Standing React Small From Right.fbx");
	conv->ExportAnimClip(0, L"Archer/Hit");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Archer/Standing Death Backward 01.fbx");
	conv->ExportAnimClip(0, L"Archer/Death");
	SafeDelete(conv);
	//Converter* conv = new Converter();
	//conv->ReadFile(L"Enemy/knight_d_pelegrini.fbx");
	//conv->ExportMaterial(L"Enemy/Mesh", false);
	//conv->ExportMesh(L"Enemy/Mesh");
	//SafeDelete(conv);

	//conv = new Converter();
	//conv->ReadFile(L"Enemy/Orc Idle.fbx");
	//conv->ExportAnimClip(0, L"Enemy/Idle");
	//SafeDelete(conv);

	//conv = new Converter();
	//conv->ReadFile(L"Enemy/Great Sword Run.fbx");
	//conv->ExportAnimClip(0, L"Enemy/Running");
	//SafeDelete(conv);

	//conv = new Converter();
	//conv->ReadFile(L"Enemy/Great Sword Slash.fbx");
	//conv->ExportAnimClip(0, L"Enemy/Attack1");
	//SafeDelete(conv);

	//conv = new Converter();
	//conv->ReadFile(L"Enemy/Great Sword Impact.fbx");
	//conv->ExportAnimClip(0, L"Enemy/Hit");
	//SafeDelete(conv);

	//conv = new Converter();
	//conv->ReadFile(L"Enemy/Falling Back Death.fbx");
	//conv->ExportAnimClip(0, L"Enemy/Death");
	//SafeDelete(conv);

	//Converter* conv = new Converter();
	//conv->ReadFile(L"Player/paladin_j_nordstrom.fbx");
	//conv->ExportMaterial(L"Player/Mesh", false);
	//conv->ExportMesh(L"Player/Mesh");
	//SafeDelete(conv);

	//conv = new Converter();
	//conv->ReadFile(L"Player/Idle.fbx");
	//conv->ExportAnimClip(0, L"Player/Idle");
	//SafeDelete(conv);

	//conv = new Converter();
	//conv->ReadFile(L"Player/Dwarf Walk.fbx");
	//conv->ExportAnimClip(0, L"Player/Walking");
	//SafeDelete(conv);

	//conv = new Converter();
	//conv->ReadFile(L"Player/Sword And Shield Run.fbx");
	//conv->ExportAnimClip(0, L"Player/Running");
	//SafeDelete(conv);

	//conv = new Converter();
	//conv->ReadFile(L"Player/Sword And Shield Slash (1).fbx");
	//conv->ExportAnimClip(0, L"Player/Attack1");
	//SafeDelete(conv);

	//conv = new Converter();
	//conv->ReadFile(L"Player/Sword And Shield Slash.fbx");
	//conv->ExportAnimClip(0, L"Player/Attack2");
	//SafeDelete(conv);

	//conv = new Converter();
	//conv->ReadFile(L"Player/Great Sword Slash.fbx");
	//conv->ExportAnimClip(0, L"Player/Attack3");
	//SafeDelete(conv);

	//conv = new Converter();
	//conv->ReadFile(L"Player/Great Sword Power Up.fbx");
	//conv->ExportAnimClip(0, L"Player/Powerup");
	//SafeDelete(conv);

	//conv = new Converter();
	//conv->ReadFile(L"Player/Sword And Shield Casting.fbx");
	//conv->ExportAnimClip(0, L"Player/SpAttack");
	//SafeDelete(conv);

	//conv = new Converter();
	//conv->ReadFile(L"Player/Standing React Large From Left.fbx");
	//conv->ExportAnimClip(0, L"Player/Hit");
	//SafeDelete(conv);

	//conv = new Converter();
	//conv->ReadFile(L"Player/Sword And Shield Death.fbx");
	//conv->ExportAnimClip(0, L"Player/Death");
	//SafeDelete(conv);

}

void ConvModel::Weapons()
{
	vector<wstring> names;
	names.push_back(L"Cutter.fbx");
	names.push_back(L"Cutter2.fbx");
	names.push_back(L"Dagger_epic.fbx");
	names.push_back(L"Dagger_small.fbx");
	names.push_back(L"Katana.fbx");
	names.push_back(L"LongArrow.obj");
	names.push_back(L"LongBow.obj");
	names.push_back(L"Rapier.fbx");
	names.push_back(L"Sword.fbx");
	names.push_back(L"Sword2.fbx");
	names.push_back(L"Sword_epic.fbx");

	for (wstring name : names)
	{
		Converter* conv = new Converter();
		conv->ReadFile(L"Weapon/" + name);


		String::Replace(&name, L".fbx", L"");
		String::Replace(&name, L".obj", L"");

		conv->ExportMaterial(L"Weapon/" + name, false);
		conv->ExportMesh(L"Weapon/" + name);
		SafeDelete(conv);
	}
}
