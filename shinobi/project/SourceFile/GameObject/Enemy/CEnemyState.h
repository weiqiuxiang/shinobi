#ifndef CENEMY_STATE_H_
#define CENEMY_STATE_H_

//先方宣言
class CEnemy;
class CEnemyStateManager;

class CEnemyState
{
public:
	CEnemyState(CEnemy*, CEnemyStateManager*) {
	}
	~CEnemyState() {}

	//オーバーライド部分
	virtual bool InitParameter(CEnemy*, CEnemyStateManager*) { return true; }
	virtual int Update(CEnemy*) { return -1; }			//戻り値 : 親オブジェクトの遷移先ステートの番号を返す、遷移はない場合-1を返す
	virtual void Draw(void) {}

	//セッター
	inline void SetState(CEnemy* pEnemy, CEnemyStateManager* pEnemyStateManager,CEnemyState* pState) {
		m_ChildState = pState;					//自分の子ともに設定
		if (m_ChildState != nullptr) {
			m_ChildState->InitParameter(pEnemy, pEnemyStateManager);
			m_ChildState->SetParentState(this);	//子供の親を自分に設定
		}
	}
	inline void SetParentState(CEnemyState* pState) {
		m_ParentState = pState;
	}

protected:
	CEnemyState() {}

protected:
	CEnemyState*			m_ParentState;
	CEnemyState*			m_ChildState;
	CEnemyStateManager*		m_EnemyStateManager;
	int						m_TransitionStateNum;		//遷移先ステート番号
};

#endif