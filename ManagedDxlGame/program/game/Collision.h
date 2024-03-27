///************Description*******************
///	衝突判定後の処理を行う
/// 
/// 
///******************************************
#pragma once
class Object;
//std::functionクラスのObjectとして扱うことができる
//std::functionは引数に任意の関数ポインタを持たせることができる
using IntersectedCall = std::function<void(std::shared_ptr<Object> a, std::shared_ptr<Object> b)>;

class PairObject {
public:
	//デフォルトコンストラクタ
	PairObject(){}
	//初期化リスト(一度だけ呼ばれる)
	//メンバ変数の初期化
	PairObject(std::shared_ptr<Object>a, std::shared_ptr<Object> b, IntersectedCall call)
		: object_a(a)
		, object_b(b)
		, intersected_call(call)

	{}
	//クラス宣言
	std::shared_ptr<Object>object_a;
	std::shared_ptr<Object>object_b;
	IntersectedCall intersected_call;

};


class Collision {
public:

	//listに追加する関数テンプレート
	//オブジェクトのペアを引数にとり、intersect_mapからそのペアに対応する関数を探して、intersect_listに追加する
	template<class A, class B>
	void RegistPairObject(std::shared_ptr<A> a, std::shared_ptr<B> b) {
		//2つのObjectの型名を１つの文字列に連結する
		std::string ab_name = std::string(typeid(A).name()) + typeid(B).name();

		//intersect_mapからそのペアに対応する関数を探す
		//intersect_mapにそのペアに対応する関数がなければ、return
		if (intersect_map.find(ab_name) == intersect_map.end())return;

		auto intersected_call = intersect_map[ab_name];
		//intersect_listに追加する
		intersect_list.emplace_back(PairObject(a, b, intersected_call));
		
	}

	//mapに追加する関数テンプレート
	//オブジェクトの型名の組み合わせと、その組み合わせに対応する関数を引数にとり、intersect_mapに登録する
	template<class A, class B>
	void RegistIntersectedProcess(const IntersectedCall& func) {
		//2つのObjectの型名を１つの文字列に連結する
		std::string ab_name = std::string(typeid(A).name()) + typeid(B).name();
		//intersect_mapにすでにその組み合わせがあれば、上書きする
		intersect_map.try_emplace(ab_name, func);
	}


	// work... AABB と AABBの判定
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

	
	//intersect_listをクリアする関数
	void ClearIntersectList();
	//intersect_mapをクリアする関数
	void ClearIntersectMap();

	//======================ゲッター================================

	//intersect_listのゲッター
	std::list<PairObject>& GetIntersectList() {
		return intersect_list;
	}

	//======================セッター================================

	//intersect_listに要素を追加する関数
	void AddIntersectList(PairObject object) {
		intersect_list.emplace_back(object);
	}

private:

	//ペアにしたオブジェクトを格納するリスト
	//衝突判定の対象となるオブジェクトのペアを格納する
	std::list<PairObject> intersect_list;

	//文字列とObjectどうしをペアにする
	//オブジェクトの型名の組み合わせと、その組み合わせに対応する関数を登録する
	std::unordered_map < std::string, IntersectedCall > intersect_map;

};