#include "raypicktomesh.h"

//関数説明 : レイピッキングによる交差判定　静止しているメッシュに対し、移動前、移動後の座標情報での判定
//引数説明 : 
//moveflag		: すべりモードのOn/Off　を　true　か　false　で渡す
//resultpos		: 衝突した場合の座標を格納する変数へのアドレス。衝突地点が必要なければ　nullptr
//pos_before	: レイ(線分)の移動前の座標(始点)
//pos_after		: レイ(線分)の移動後の座標(終点)
//mesh			: 当たり判定を取るメッシュの頂点情報
//matrix		: 当たり判定を取るメッシュを変換する行列
//戻り値説明 : bool型　当ったときにtrue　当っていないときにfalseを返す
bool RayPickToMesh::RaypicktoMesh( bool moveflag , D3DXVECTOR3 *resultpos, D3DXVECTOR3 pos_before , D3DXVECTOR3 pos_after , LPD3DXMESH mesh , D3DXMATRIX matrix )
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//	ポリゴンにあたってすべる処理
	//
	
	//	交差判定：オブジェクトのローカル座標系で当たり判定を行うため、
	//	絶対座標の位置情報を交差判定をするオブジェクトを変換した行列の逆行列を使ってする
	BOOL hit;DWORD index;float dist;					//	交差判定情報受け取り変数
	D3DXVECTOR3 _direction;				//	変換後の位置、方向を格納する変数：本当はここで定義すべし表示用にグローバル定義にしている
	D3DXMATRIX invmat;									//	逆行列を格納する変数
	D3DXVECTOR3 vertex[3],vertexnormal[3];				//　1ポリゴンを構成する3頂点の座標と方向
	D3DXVECTOR3 polyNormal;								//	ポリゴンの法線（方向）
	D3DXPLANE	plane;									//	平面処理用の構造体：ポリゴンを平面として処理するため


	D3DXMatrixInverse ( &invmat , nullptr , &matrix );	//	逆行列の取得
	D3DXVec3TransformCoord(&pos_before,&pos_before,&invmat);	//	逆行列を使用し、レイ始点情報を変換　位置と向きで変換する関数が異なるので要注意
	D3DXVec3TransformCoord(&pos_after,&pos_after,&invmat);		//	逆行列を使用し、レイ終点情報を変換　位置と向きで変換する関数が異なるので要注意

	D3DXVECTOR3 VecAfterToBefore = pos_after - pos_before;
	D3DXVec3Normalize (&_direction ,&VecAfterToBefore);					//	逆行列を使用し、レイ方向情報を変換　位置と向きで変換する関数が異なるので要注意
	
	//	移動方向レイとの当たり判定を行う
	D3DXIntersect (mesh,&pos_before,&_direction,&hit,&index,nullptr,nullptr,&dist,nullptr,nullptr );

	//	これは当たり判定のチェックではない点に注意：あくまで移動方向にポリゴンがあるかどうかを判断している
	if( hit )
	{

		//	移動方向の先には、この番号のポリゴンに当たるらしい
		int polyindex = index;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		//	メッシュと取得し、頂点フォーマットなどの情報を取得
		//	メッシュから、当たり判定に使用することになるポリゴンのの情報を取得する
		//	このあたりの処理はほぼこれで定型と思っていい
		//
		LPD3DXMESH pMesh = mesh;			//	メッシュクラスから実メッシュを取得
		DWORD fvf = pMesh->GetFVF();							//	実メッシュから頂点情報を取得

		//	インデックスバッファと頂点バッファの取得
		//	インデックスバッファとは1ポリゴンを構成するのにどのポリゴンがしようされているかを表す：通常3点で1ポリを構成する
		//LPDIRECT3DINDEXBUFFER9	pI;								//	インデックスバッファ
		//D3DINDEXBUFFER_DESC		Idesc;							//	インデックスバッファの構成情報
		LPVOID pVB,pIB;											//	形式なしポインタ、まずこの形式でポインタを取得する
		_XFileIndexBuffer *i_pt;								//	自分が作ったインデックスバッファ処理用構造体へのポインタ
		

//		pMesh->GetIndexBuffer( &pI );							//	インデックスバッファを取得	
//		pI->GetDesc( &Idesc );									//	インデックスバッファから構成情報を取得
		pMesh->LockIndexBuffer( D3DLOCK_READONLY , &pIB );		//	インデックスバッファをロックしポインタを取得
		pMesh->LockVertexBuffer( D3DLOCK_READONLY , &pVB );		//	バーテックスバッファをロックしポインタを取得
		pMesh->UnlockIndexBuffer();								//	ロックして用がすんだらすぐアンロック
		pMesh->UnlockVertexBuffer();							//	ロックして用がすんだらすぐアンロック
		
		//	インデックスバッファを値取得用構造体にキャスト
		i_pt = (_XFileIndexBuffer*)pIB;							//	こうすることにより、ただのメモリ領域を自分が規定したデータ形式で処理できる、これがC言語の一番強力な機能である
		//	必要なインデックスまで進める
		for( int i=0 ; i<polyindex ; i++ )
		{
			i_pt++;
		}
		
		//	頂点フォーマットによってキャストする構造体を変更する
		if( fvf == ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 ) )		//	位置：方向：テクスチャ1枚の場合
		{
			//	無形ポインタを自分の作成したバーテックスバッファ処理用のポインタにキャストする
			//	こうすることにより、ただのメモリ領域を自分が規定したデータ形式で処理できる、これがC言語の一番強力な機能である	
			_XFileVertexBuffer_p_n_t *v_pt;								//	位置：方向：テクスチャ1枚の構造体のポインタ			
			v_pt = (_XFileVertexBuffer_p_n_t*)pVB;						//	キャスト

			//	3頂点のデータを取得する
			for( int i=0 ; i<3 ; i++ )
			{
				//	頂点バッファの先頭アドレス取得
				_XFileVertexBuffer_p_n_t *w_pt = v_pt;

				//	取得した頂点番号までポインタをスキップ
				for( int j=0 ; j<i_pt->p[i] ; j++ ) w_pt++;

				//	該当する頂点の法線情報取得と頂点位置情報を取得
				vertexnormal[i] = w_pt->normal;							//	方向情報
				vertex[i] = w_pt->pos;									//	位置情報
			}
		}
		if( fvf == ( D3DFVF_XYZ | D3DFVF_NORMAL) )						//	位置：方向の場合
		{
			//	無形ポインタを自分の作成したバーテックスバッファ処理用のポインタにキャストする
			//	こうすることにより、ただのメモリ領域を自分が規定したデータ形式で処理できる、これがC言語の一番強力な機能である	
			_XFileVertexBuffer_p_n *v_pt;								//	位置：方向構造体のポインタ			
			v_pt = (_XFileVertexBuffer_p_n*)pVB;						//	キャスト

			//	3頂点のデータを取得する
			for( int i=0 ; i<3 ; i++ )
			{
				//	頂点バッファの先頭アドレス取得
				_XFileVertexBuffer_p_n *w_pt = v_pt;

				//	取得した頂点番号までポインタをスキップ
				for( int j=0 ; j<i_pt->p[i] ; j++ ) w_pt++;

				//	該当する頂点の法線情報取得と頂点位置情報を取得
				vertexnormal[i] = w_pt->normal;							//	方向情報
				vertex[i] = w_pt->pos;									//	位置情報
			}
		}
		
		//	取得した3点の頂点データから、ポリゴンの向きを求め、そこからすべり後の座標を求める
		//	取得した3頂点のデータからポリゴンの法線を求める
		polyNormal.x = ( vertexnormal[0].x + vertexnormal[1].x + vertexnormal[2].x)/3;
		polyNormal.y = ( vertexnormal[0].y + vertexnormal[1].y + vertexnormal[2].y)/3;
		polyNormal.z = ( vertexnormal[0].z + vertexnormal[1].z + vertexnormal[2].z)/3;
		
		//	3点を用いて無限平面を定義する
		D3DXPlaneFromPoints( &plane,&vertex[0],&vertex[1],&vertex[2]);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		//	移動前後の座標がポリゴンを挟んでいるか判定し、はさんでいればアタットミなし、すべり処理を行う
		//	このあたりからは実際の移動前後の座標を使用するので注意する

		//	移動後、前の位置が平面をはさむかチェック
		float ans[2];	//	数学の平面と頂点に関する公式を利用する：平面の表面にあるなら正：裏面なら負になる
		ans[0] =	( plane.a * pos_after.x ) +(	plane.b * pos_after.y )+(plane.c * pos_after.z )+plane.d;
		ans[1] =	( plane.a * pos_before.x ) +(	plane.b * pos_before.y )+(plane.c * pos_before.z )+plane.d;

		//	二つの値の正負が反対ならば、両面をはさむ->当たっているのですべり処理を行う
		if(  ans[0]*ans[1] < 0.0f )
		{
			//	ヒットしていた場合にすべりモードが有効でなければ、衝突位置を返して終了
			if( moveflag == false )
			{
				if( resultpos )
				{
					*resultpos = pos_before + _direction*(dist+0.05f);
					//	あたり判定オブジェクトのローカル座標系での位置なので、絶対座標に変換しなおす
					D3DXVec3TransformCoord(resultpos,resultpos,&matrix);
				}
			}
			else
			{
				//	すべり後の座標が入る変数
				D3DXVECTOR3 afterpos;
				//	移動後の座標からポリゴン法線方向にレイを飛ばし、ポリゴンとの交点を求める
				D3DXIntersect (mesh,&pos_after,&polyNormal,&hit,&index,nullptr,nullptr,&dist,nullptr,nullptr );
				//	交差していた場合は、レイ表示情報の終点位置を更新する
				if( hit )
				{
					//	すべり後の座標ポリゴンの表面より少し多めに戻す
					afterpos = 	pos_after+polyNormal*(dist+0.05f);
					
					//	あたり判定オブジェクトのローカル座標系での位置なので、絶対座標に変換しなおす
					D3DXVec3TransformCoord(&afterpos,&afterpos,&matrix);
					
					//	すべての処理がすんだ座標をセット
					if( resultpos )
					{
						*resultpos = afterpos;
					}
				}
			}
			return true;	
		}

		//	2つの値が負の場合、オブジェクトに埋まってしまっているので、反対方向の直近ポリゴンまで移動
		if(  ans[0] < 0.0f && ans[1] < 0.0f )
		{
			VecAfterToBefore = pos_before - pos_after;

			//	反対方向にレイを飛ばす
			D3DXVec3Normalize (&_direction ,&VecAfterToBefore);
			//	移動方向レイとの当たり判定を行う
			D3DXIntersect (mesh,&pos_after,&_direction,&hit,&index,nullptr,nullptr,&dist,nullptr,nullptr );
			
			if( hit )
			{
				if( resultpos )
				{
					*resultpos = pos_after + _direction*(dist+0.05f);
					//	あたり判定オブジェクトのローカル座標系での位置なので、絶対座標に変換しなおす
					D3DXVec3TransformCoord(resultpos,resultpos,&matrix);
				}
				return true;	
			}

		}
	}
	
	//	ここまできたら当たっていないのでfalseを返す
	return false;
}

//	レイピッキングによる交差判定　判定座標が静止しており、メッシュの移動前、移動後の行列情報での判定(レイは動かずにつまり点になるってこと,だけとメッシュが動いている)
bool RayPickToMesh::RaypicktoMesh( bool moveflag , D3DXVECTOR3 *resultpos, D3DXVECTOR3 pos , LPD3DXMESH mesh , D3DXMATRIX matrix_befoe , D3DXMATRIX matrix_afiter )
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//	ポリゴンにあたってすべる処理
	//
	D3DXVECTOR3 pos_before,pos_after;
	D3DXMATRIX matrix = matrix_afiter;

	//	交差判定：オブジェクトのローカル座標系で当たり判定を行うため、
	//	絶対座標の位置情報を交差判定をするオブジェクトを変換した行列の逆行列を使ってする
	BOOL hit;DWORD index;float dist;					//	交差判定情報受け取り変数
	D3DXVECTOR3 _direction;				//	変換後の位置、方向を格納する変数：本当はここで定義すべし表示用にグローバル定義にしている
	D3DXMATRIX invmat_b,invmat_a;									//	逆行列を格納する変数
	D3DXVECTOR3 vertex[3],vertexnormal[3];				//　1ポリゴンを構成する3頂点の座標と方向
	D3DXVECTOR3 polyNormal;								//	ポリゴンの法線（方向）
	D3DXPLANE	plane;									//	平面処理用の構造体：ポリゴンを平面として処理するため

	D3DXMatrixInverse ( &invmat_b , nullptr , &matrix_befoe );	//	逆行列の取得
	D3DXMatrixInverse ( &invmat_a , nullptr , &matrix_afiter );	
	
	//	メッシュから見た前フレームの位置と現在フレームの位置を算出

	D3DXVec3TransformCoord(&pos_before,&pos,&invmat_b);
	D3DXVec3TransformCoord(&pos_after,&pos,&invmat_a);

	//D3DXVec3TransformCoord(&pos_before,&pos_before,&invmat);	//	逆行列を使用し、レイ始点情報を変換　位置と向きで変換する関数が異なるので要注意
	//D3DXVec3TransformCoord(&pos_after,&pos_after,&invmat);		//	逆行列を使用し、レイ終点情報を変換　位置と向きで変換する関数が異なるので要注意

	D3DXVECTOR3 VecAfterToBefore = pos_after - pos_before;
	D3DXVec3Normalize (&_direction ,&VecAfterToBefore);				//	逆行列を使用し、レイ方向情報を変換　位置と向きで変換する関数が異なるので要注意
	
	//	移動方向レイとの当たり判定を行う
	D3DXIntersect (mesh,&pos_before,&_direction,&hit,&index,nullptr,nullptr,&dist,nullptr,nullptr );

	//	これは当たり判定のチェックではない点に注意：あくまで移動方向にポリゴンがあるかどうかを判断している
	if( hit )
	{

		//	移動方向の先には、この番号のポリゴンに当たるらしい
		int polyindex = index;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		//	メッシュと取得し、頂点フォーマットなどの情報を取得
		//	メッシュから、当たり判定に使用することになるポリゴンのの情報を取得する
		//	このあたりの処理はほぼこれで定型と思っていい
		//
		LPD3DXMESH pMesh = mesh;			//	メッシュクラスから実メッシュを取得
		DWORD fvf = pMesh->GetFVF();							//	実メッシュから頂点情報を取得

		//	インデックスバッファと頂点バッファの取得
		//	インデックスバッファとは1ポリゴンを構成するのにどのポリゴンがしようされているかを表す：通常3点で1ポリを構成する
		//LPDIRECT3DINDEXBUFFER9	pI;								//	インデックスバッファ
		//D3DINDEXBUFFER_DESC		Idesc;							//	インデックスバッファの構成情報
		LPVOID pVB,pIB;											//	形式なしポインタ、まずこの形式でポインタを取得する
		_XFileIndexBuffer *i_pt;								//	自分が作ったインデックスバッファ処理用構造体へのポインタ
		

//		pMesh->GetIndexBuffer( &pI );							//	インデックスバッファを取得	
//		pI->GetDesc( &Idesc );									//	インデックスバッファから構成情報を取得
		pMesh->LockIndexBuffer( D3DLOCK_READONLY , &pIB );		//	インデックスバッファをロックしポインタを取得
		pMesh->LockVertexBuffer( D3DLOCK_READONLY , &pVB );		//	バーテックスバッファをロックしポインタを取得
		pMesh->UnlockIndexBuffer();								//	ロックして用がすんだらすぐアンロック
		pMesh->UnlockVertexBuffer();							//	ロックして用がすんだらすぐアンロック
		
		//	インデックスバッファを値取得用構造体にキャスト
		i_pt = (_XFileIndexBuffer*)pIB;							//	こうすることにより、ただのメモリ領域を自分が規定したデータ形式で処理できる、これがC言語の一番強力な機能である
		//	必要なインデックスまで進める
		for( int i=0 ; i<polyindex ; i++ )
		{
			i_pt++;
		}
		
		//	頂点フォーマットによってキャストする構造体を変更する
		if( fvf == ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 ) )		//	位置：方向：テクスチャ1枚の場合
		{
			//	無形ポインタを自分の作成したバーテックスバッファ処理用のポインタにキャストする
			//	こうすることにより、ただのメモリ領域を自分が規定したデータ形式で処理できる、これがC言語の一番強力な機能である	
			_XFileVertexBuffer_p_n_t *v_pt;								//	位置：方向：テクスチャ1枚の構造体のポインタ			
			v_pt = (_XFileVertexBuffer_p_n_t*)pVB;						//	キャスト

			//	3頂点のデータを取得する
			for( int i=0 ; i<3 ; i++ )
			{
				//	頂点バッファの先頭アドレス取得
				_XFileVertexBuffer_p_n_t *w_pt = v_pt;

				//	取得した頂点番号までポインタをスキップ
				for( int j=0 ; j<i_pt->p[i] ; j++ ) w_pt++;

				//	該当する頂点の法線情報取得と頂点位置情報を取得
				vertexnormal[i] = w_pt->normal;							//	方向情報
				vertex[i] = w_pt->pos;									//	位置情報
			}
		}
		if( fvf == ( D3DFVF_XYZ | D3DFVF_NORMAL) )						//	位置：方向の場合
		{
			//	無形ポインタを自分の作成したバーテックスバッファ処理用のポインタにキャストする
			//	こうすることにより、ただのメモリ領域を自分が規定したデータ形式で処理できる、これがC言語の一番強力な機能である	
			_XFileVertexBuffer_p_n *v_pt;								//	位置：方向構造体のポインタ			
			v_pt = (_XFileVertexBuffer_p_n*)pVB;						//	キャスト

			//	3頂点のデータを取得する
			for( int i=0 ; i<3 ; i++ )
			{
				//	頂点バッファの先頭アドレス取得
				_XFileVertexBuffer_p_n *w_pt = v_pt;

				//	取得した頂点番号までポインタをスキップ
				for( int j=0 ; j<i_pt->p[i] ; j++ ) w_pt++;

				//	該当する頂点の法線情報取得と頂点位置情報を取得
				vertexnormal[i] = w_pt->normal;							//	方向情報
				vertex[i] = w_pt->pos;									//	位置情報
			}
		}
		
		//	取得した3点の頂点データから、ポリゴンの向きを求め、そこからすべり後の座標を求める
		//	取得した3頂点のデータからポリゴンの法線を求める
		polyNormal.x = ( vertexnormal[0].x + vertexnormal[1].x + vertexnormal[2].x)/3;
		polyNormal.y = ( vertexnormal[0].y + vertexnormal[1].y + vertexnormal[2].y)/3;
		polyNormal.z = ( vertexnormal[0].z + vertexnormal[1].z + vertexnormal[2].z)/3;
		
		//	3点を用いて無限平面を定義する
		D3DXPlaneFromPoints( &plane,&vertex[0],&vertex[1],&vertex[2]);

		


		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		//	移動前後の座標がポリゴンを挟んでいるか判定し、はさんでいればアタットミなし、すべり処理を行う
		//	このあたりからは実際の移動前後の座標を使用するので注意する

		//	移動後、前の位置が平面をはさむかチェック
		float ans[2];	//	数学の平面と頂点に関する公式を利用する：平面の表面にあるなら正：裏面なら負になる
		ans[0] =	( plane.a * pos_after.x ) +(	plane.b * pos_after.y )+(plane.c * pos_after.z )+plane.d;
		ans[1] =	( plane.a * pos_before.x ) +(	plane.b * pos_before.y )+(plane.c * pos_before.z )+plane.d;

		//	二つの値の正負が反対ならば、両面をはさむ->当たっているのですべり処理を行う
		if(  ans[0]*ans[1] < 0.0f )
		{
			//	ヒットしていた場合にすべりモードが有効でなければ、衝突位置を返して終了
			if( moveflag == false )
			{
				if( resultpos )
				{
					*resultpos = pos_before + _direction*(dist+0.05f);
					//	あたり判定オブジェクトのローカル座標系での位置なので、絶対座標に変換しなおす
					D3DXVec3TransformCoord(resultpos,resultpos,&matrix);
				}
			}
			else
			{
				//	すべり後の座標が入る変数
				D3DXVECTOR3 afterpos;
				//	移動後の座標からポリゴン法線方向にレイを飛ばし、ポリゴンとの交点を求める
				D3DXIntersect (mesh,&pos_after,&polyNormal,&hit,&index,nullptr,nullptr,&dist,nullptr,nullptr );
				//	交差していた場合は、レイ表示情報の終点位置を更新する
				if( hit )
				{
					//	すべり後の座標ポリゴンの表面より少し多めに戻す
					afterpos = 	pos_after+polyNormal*(dist+0.05f);
					
					//	あたり判定オブジェクトのローカル座標系での位置なので、絶対座標に変換しなおす
					D3DXVec3TransformCoord(&afterpos,&afterpos,&matrix);
					
					//	すべての処理がすんだ座標をセット
					if( resultpos )
					{
						*resultpos = afterpos;
					}
				}
			}
			return true;	
		}

		//	2つの値が負の場合、オブジェクトに埋まってしまっているので、反対方向の直近ポリゴンまで移動
		if(  ans[0] < 0.0f && ans[1] < 0.0f )
		{
			VecAfterToBefore = pos_before - pos_after;

			//	反対方向にレイを飛ばす
			D3DXVec3Normalize (&_direction ,&VecAfterToBefore);
			//	移動方向レイとの当たり判定を行う
			D3DXIntersect (mesh,&pos_after,&_direction,&hit,&index,nullptr,nullptr,&dist,nullptr,nullptr );
			
			if( hit )
			{
				if( resultpos )
				{
					*resultpos = pos_after + _direction*(dist+0.05f);
					//	あたり判定オブジェクトのローカル座標系での位置なので、絶対座標に変換しなおす
					D3DXVec3TransformCoord(resultpos,resultpos,&matrix);
				}
				return true;	
			}

		}
	}
	
	//	ここまできたら当たっていないのでfalseを返す
	return false;
}










