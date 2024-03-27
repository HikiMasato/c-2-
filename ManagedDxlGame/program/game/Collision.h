///************Description*******************
///	�Փ˔����̏������s��
/// 
/// 
///******************************************
#pragma once
class Object;
//std::function�N���X��Object�Ƃ��Ĉ������Ƃ��ł���
//std::function�͈����ɔC�ӂ̊֐��|�C���^���������邱�Ƃ��ł���
using IntersectedCall = std::function<void(std::shared_ptr<Object> a, std::shared_ptr<Object> b)>;

class PairObject {
public:
	//�f�t�H���g�R���X�g���N�^
	PairObject(){}
	//���������X�g(��x�����Ă΂��)
	//�����o�ϐ��̏�����
	PairObject(std::shared_ptr<Object>a, std::shared_ptr<Object> b, IntersectedCall call)
		: object_a(a)
		, object_b(b)
		, intersected_call(call)

	{}
	//�N���X�錾
	std::shared_ptr<Object>object_a;
	std::shared_ptr<Object>object_b;
	IntersectedCall intersected_call;

};


class Collision {
public:

	//list�ɒǉ�����֐��e���v���[�g
	//�I�u�W�F�N�g�̃y�A�������ɂƂ�Aintersect_map���炻�̃y�A�ɑΉ�����֐���T���āAintersect_list�ɒǉ�����
	template<class A, class B>
	void RegistPairObject(std::shared_ptr<A> a, std::shared_ptr<B> b) {
		//2��Object�̌^�����P�̕�����ɘA������
		std::string ab_name = std::string(typeid(A).name()) + typeid(B).name();

		//intersect_map���炻�̃y�A�ɑΉ�����֐���T��
		//intersect_map�ɂ��̃y�A�ɑΉ�����֐����Ȃ���΁Areturn
		if (intersect_map.find(ab_name) == intersect_map.end())return;

		auto intersected_call = intersect_map[ab_name];
		//intersect_list�ɒǉ�����
		intersect_list.emplace_back(PairObject(a, b, intersected_call));
		
	}

	//map�ɒǉ�����֐��e���v���[�g
	//�I�u�W�F�N�g�̌^���̑g�ݍ��킹�ƁA���̑g�ݍ��킹�ɑΉ�����֐��������ɂƂ�Aintersect_map�ɓo�^����
	template<class A, class B>
	void RegistIntersectedProcess(const IntersectedCall& func) {
		//2��Object�̌^�����P�̕�����ɘA������
		std::string ab_name = std::string(typeid(A).name()) + typeid(B).name();
		//intersect_map�ɂ��łɂ��̑g�ݍ��킹������΁A�㏑������
		intersect_map.try_emplace(ab_name, func);
	}


	// work... AABB �� AABB�̔���
	bool IsIntersectAABB(const tnl::Vector3& a, const tnl::Vector3& a_size, const tnl::Vector3& b, const tnl::Vector3& b_size)
	{
		tnl::Vector3 a_max = tnl::ToMaxAABB(a, a_size);
		tnl::Vector3 a_min = tnl::ToMinAABB(a, a_size);
		tnl::Vector3 b_max = tnl::ToMaxAABB(b, b_size);
		tnl::Vector3 b_min = tnl::ToMinAABB(b, b_size);
		if (a_max.x < b_min.x || a_min.x > b_max.x) return false;
		if (a_max.y < b_min.y || a_min.y > b_max.y) return false;
		if (a_max.z < b_min.z || a_min.z > b_max.z) return false;
		return true;
	}

	
	//intersect_list���N���A����֐�
	void ClearIntersectList();
	//intersect_map���N���A����֐�
	void ClearIntersectMap();

	//======================�Q�b�^�[================================

	//intersect_list�̃Q�b�^�[
	std::list<PairObject>& GetIntersectList() {
		return intersect_list;
	}

	//======================�Z�b�^�[================================

	//intersect_list�ɗv�f��ǉ�����֐�
	void AddIntersectList(PairObject object) {
		intersect_list.emplace_back(object);
	}

private:

	//�y�A�ɂ����I�u�W�F�N�g���i�[���郊�X�g
	//�Փ˔���̑ΏۂƂȂ�I�u�W�F�N�g�̃y�A���i�[����
	std::list<PairObject> intersect_list;

	//�������Object�ǂ������y�A�ɂ���
	//�I�u�W�F�N�g�̌^���̑g�ݍ��킹�ƁA���̑g�ݍ��킹�ɑΉ�����֐���o�^����
	std::unordered_map < std::string, IntersectedCall > intersect_map;

};