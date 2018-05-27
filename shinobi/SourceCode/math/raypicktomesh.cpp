#include "raypicktomesh.h"

//�֐����� : ���C�s�b�L���O�ɂ���������@�Î~���Ă��郁�b�V���ɑ΂��A�ړ��O�A�ړ���̍��W���ł̔���
//�������� : 
//moveflag		: ���ׂ胂�[�h��On/Off�@���@true�@���@false�@�œn��
//resultpos		: �Փ˂����ꍇ�̍��W���i�[����ϐ��ւ̃A�h���X�B�Փ˒n�_���K�v�Ȃ���΁@nullptr
//pos_before	: ���C(����)�̈ړ��O�̍��W(�n�_)
//pos_after		: ���C(����)�̈ړ���̍��W(�I�_)
//mesh			: �����蔻�����郁�b�V���̒��_���
//matrix		: �����蔻�����郁�b�V����ϊ�����s��
//�߂�l���� : bool�^�@�������Ƃ���true�@�����Ă��Ȃ��Ƃ���false��Ԃ�
bool RayPickToMesh::RaypicktoMesh( bool moveflag , D3DXVECTOR3 *resultpos, D3DXVECTOR3 pos_before , D3DXVECTOR3 pos_after , LPD3DXMESH mesh , D3DXMATRIX matrix )
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//	�|���S���ɂ������Ă��ׂ鏈��
	//
	
	//	��������F�I�u�W�F�N�g�̃��[�J�����W�n�œ����蔻����s�����߁A
	//	��΍��W�̈ʒu�����������������I�u�W�F�N�g��ϊ������s��̋t�s����g���Ă���
	BOOL hit;DWORD index;float dist;					//	����������󂯎��ϐ�
	D3DXVECTOR3 _direction;				//	�ϊ���̈ʒu�A�������i�[����ϐ��F�{���͂����Œ�`���ׂ��\���p�ɃO���[�o����`�ɂ��Ă���
	D3DXMATRIX invmat;									//	�t�s����i�[����ϐ�
	D3DXVECTOR3 vertex[3],vertexnormal[3];				//�@1�|���S�����\������3���_�̍��W�ƕ���
	D3DXVECTOR3 polyNormal;								//	�|���S���̖@���i�����j
	D3DXPLANE	plane;									//	���ʏ����p�̍\���́F�|���S���𕽖ʂƂ��ď������邽��


	D3DXMatrixInverse ( &invmat , nullptr , &matrix );	//	�t�s��̎擾
	D3DXVec3TransformCoord(&pos_before,&pos_before,&invmat);	//	�t�s����g�p���A���C�n�_����ϊ��@�ʒu�ƌ����ŕϊ�����֐����قȂ�̂ŗv����
	D3DXVec3TransformCoord(&pos_after,&pos_after,&invmat);		//	�t�s����g�p���A���C�I�_����ϊ��@�ʒu�ƌ����ŕϊ�����֐����قȂ�̂ŗv����

	D3DXVECTOR3 VecAfterToBefore = pos_after - pos_before;
	D3DXVec3Normalize (&_direction ,&VecAfterToBefore);					//	�t�s����g�p���A���C��������ϊ��@�ʒu�ƌ����ŕϊ�����֐����قȂ�̂ŗv����
	
	//	�ړ��������C�Ƃ̓����蔻����s��
	D3DXIntersect (mesh,&pos_before,&_direction,&hit,&index,nullptr,nullptr,&dist,nullptr,nullptr );

	//	����͓����蔻��̃`�F�b�N�ł͂Ȃ��_�ɒ��ӁF�����܂ňړ������Ƀ|���S�������邩�ǂ����𔻒f���Ă���
	if( hit )
	{

		//	�ړ������̐�ɂ́A���̔ԍ��̃|���S���ɓ�����炵��
		int polyindex = index;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		//	���b�V���Ǝ擾���A���_�t�H�[�}�b�g�Ȃǂ̏����擾
		//	���b�V������A�����蔻��Ɏg�p���邱�ƂɂȂ�|���S���̂̏����擾����
		//	���̂�����̏����͂قڂ���Œ�^�Ǝv���Ă���
		//
		LPD3DXMESH pMesh = mesh;			//	���b�V���N���X��������b�V�����擾
		DWORD fvf = pMesh->GetFVF();							//	�����b�V�����璸�_�����擾

		//	�C���f�b�N�X�o�b�t�@�ƒ��_�o�b�t�@�̎擾
		//	�C���f�b�N�X�o�b�t�@�Ƃ�1�|���S�����\������̂ɂǂ̃|���S�������悤����Ă��邩��\���F�ʏ�3�_��1�|�����\������
		//LPDIRECT3DINDEXBUFFER9	pI;								//	�C���f�b�N�X�o�b�t�@
		//D3DINDEXBUFFER_DESC		Idesc;							//	�C���f�b�N�X�o�b�t�@�̍\�����
		LPVOID pVB,pIB;											//	�`���Ȃ��|�C���^�A�܂����̌`���Ń|�C���^���擾����
		_XFileIndexBuffer *i_pt;								//	������������C���f�b�N�X�o�b�t�@�����p�\���̂ւ̃|�C���^
		

//		pMesh->GetIndexBuffer( &pI );							//	�C���f�b�N�X�o�b�t�@���擾	
//		pI->GetDesc( &Idesc );									//	�C���f�b�N�X�o�b�t�@����\�������擾
		pMesh->LockIndexBuffer( D3DLOCK_READONLY , &pIB );		//	�C���f�b�N�X�o�b�t�@�����b�N���|�C���^���擾
		pMesh->LockVertexBuffer( D3DLOCK_READONLY , &pVB );		//	�o�[�e�b�N�X�o�b�t�@�����b�N���|�C���^���擾
		pMesh->UnlockIndexBuffer();								//	���b�N���ėp�����񂾂炷���A�����b�N
		pMesh->UnlockVertexBuffer();							//	���b�N���ėp�����񂾂炷���A�����b�N
		
		//	�C���f�b�N�X�o�b�t�@��l�擾�p�\���̂ɃL���X�g
		i_pt = (_XFileIndexBuffer*)pIB;							//	�������邱�Ƃɂ��A�����̃������̈���������K�肵���f�[�^�`���ŏ����ł���A���ꂪC����̈�ԋ��͂ȋ@�\�ł���
		//	�K�v�ȃC���f�b�N�X�܂Ői�߂�
		for( int i=0 ; i<polyindex ; i++ )
		{
			i_pt++;
		}
		
		//	���_�t�H�[�}�b�g�ɂ���ăL���X�g����\���̂�ύX����
		if( fvf == ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 ) )		//	�ʒu�F�����F�e�N�X�`��1���̏ꍇ
		{
			//	���`�|�C���^�������̍쐬�����o�[�e�b�N�X�o�b�t�@�����p�̃|�C���^�ɃL���X�g����
			//	�������邱�Ƃɂ��A�����̃������̈���������K�肵���f�[�^�`���ŏ����ł���A���ꂪC����̈�ԋ��͂ȋ@�\�ł���	
			_XFileVertexBuffer_p_n_t *v_pt;								//	�ʒu�F�����F�e�N�X�`��1���̍\���̂̃|�C���^			
			v_pt = (_XFileVertexBuffer_p_n_t*)pVB;						//	�L���X�g

			//	3���_�̃f�[�^���擾����
			for( int i=0 ; i<3 ; i++ )
			{
				//	���_�o�b�t�@�̐擪�A�h���X�擾
				_XFileVertexBuffer_p_n_t *w_pt = v_pt;

				//	�擾�������_�ԍ��܂Ń|�C���^���X�L�b�v
				for( int j=0 ; j<i_pt->p[i] ; j++ ) w_pt++;

				//	�Y�����钸�_�̖@�����擾�ƒ��_�ʒu�����擾
				vertexnormal[i] = w_pt->normal;							//	�������
				vertex[i] = w_pt->pos;									//	�ʒu���
			}
		}
		if( fvf == ( D3DFVF_XYZ | D3DFVF_NORMAL) )						//	�ʒu�F�����̏ꍇ
		{
			//	���`�|�C���^�������̍쐬�����o�[�e�b�N�X�o�b�t�@�����p�̃|�C���^�ɃL���X�g����
			//	�������邱�Ƃɂ��A�����̃������̈���������K�肵���f�[�^�`���ŏ����ł���A���ꂪC����̈�ԋ��͂ȋ@�\�ł���	
			_XFileVertexBuffer_p_n *v_pt;								//	�ʒu�F�����\���̂̃|�C���^			
			v_pt = (_XFileVertexBuffer_p_n*)pVB;						//	�L���X�g

			//	3���_�̃f�[�^���擾����
			for( int i=0 ; i<3 ; i++ )
			{
				//	���_�o�b�t�@�̐擪�A�h���X�擾
				_XFileVertexBuffer_p_n *w_pt = v_pt;

				//	�擾�������_�ԍ��܂Ń|�C���^���X�L�b�v
				for( int j=0 ; j<i_pt->p[i] ; j++ ) w_pt++;

				//	�Y�����钸�_�̖@�����擾�ƒ��_�ʒu�����擾
				vertexnormal[i] = w_pt->normal;							//	�������
				vertex[i] = w_pt->pos;									//	�ʒu���
			}
		}
		
		//	�擾����3�_�̒��_�f�[�^����A�|���S���̌��������߁A�������炷�ׂ��̍��W�����߂�
		//	�擾����3���_�̃f�[�^����|���S���̖@�������߂�
		polyNormal.x = ( vertexnormal[0].x + vertexnormal[1].x + vertexnormal[2].x)/3;
		polyNormal.y = ( vertexnormal[0].y + vertexnormal[1].y + vertexnormal[2].y)/3;
		polyNormal.z = ( vertexnormal[0].z + vertexnormal[1].z + vertexnormal[2].z)/3;
		
		//	3�_��p���Ė������ʂ��`����
		D3DXPlaneFromPoints( &plane,&vertex[0],&vertex[1],&vertex[2]);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		//	�ړ��O��̍��W���|���S��������ł��邩���肵�A�͂���ł���΃A�^�b�g�~�Ȃ��A���ׂ菈�����s��
		//	���̂����肩��͎��ۂ̈ړ��O��̍��W���g�p����̂Œ��ӂ���

		//	�ړ���A�O�̈ʒu�����ʂ��͂��ނ��`�F�b�N
		float ans[2];	//	���w�̕��ʂƒ��_�Ɋւ�������𗘗p����F���ʂ̕\�ʂɂ���Ȃ琳�F���ʂȂ畉�ɂȂ�
		ans[0] =	( plane.a * pos_after.x ) +(	plane.b * pos_after.y )+(plane.c * pos_after.z )+plane.d;
		ans[1] =	( plane.a * pos_before.x ) +(	plane.b * pos_before.y )+(plane.c * pos_before.z )+plane.d;

		//	��̒l�̐��������΂Ȃ�΁A���ʂ��͂���->�������Ă���̂ł��ׂ菈�����s��
		if(  ans[0]*ans[1] < 0.0f )
		{
			//	�q�b�g���Ă����ꍇ�ɂ��ׂ胂�[�h���L���łȂ���΁A�Փˈʒu��Ԃ��ďI��
			if( moveflag == false )
			{
				if( resultpos )
				{
					*resultpos = pos_before + _direction*(dist+0.05f);
					//	�����蔻��I�u�W�F�N�g�̃��[�J�����W�n�ł̈ʒu�Ȃ̂ŁA��΍��W�ɕϊ����Ȃ���
					D3DXVec3TransformCoord(resultpos,resultpos,&matrix);
				}
			}
			else
			{
				//	���ׂ��̍��W������ϐ�
				D3DXVECTOR3 afterpos;
				//	�ړ���̍��W����|���S���@�������Ƀ��C���΂��A�|���S���Ƃ̌�_�����߂�
				D3DXIntersect (mesh,&pos_after,&polyNormal,&hit,&index,nullptr,nullptr,&dist,nullptr,nullptr );
				//	�������Ă����ꍇ�́A���C�\�����̏I�_�ʒu���X�V����
				if( hit )
				{
					//	���ׂ��̍��W�|���S���̕\�ʂ�菭�����߂ɖ߂�
					afterpos = 	pos_after+polyNormal*(dist+0.05f);
					
					//	�����蔻��I�u�W�F�N�g�̃��[�J�����W�n�ł̈ʒu�Ȃ̂ŁA��΍��W�ɕϊ����Ȃ���
					D3DXVec3TransformCoord(&afterpos,&afterpos,&matrix);
					
					//	���ׂĂ̏��������񂾍��W���Z�b�g
					if( resultpos )
					{
						*resultpos = afterpos;
					}
				}
			}
			return true;	
		}

		//	2�̒l�����̏ꍇ�A�I�u�W�F�N�g�ɖ��܂��Ă��܂��Ă���̂ŁA���Ε����̒��߃|���S���܂ňړ�
		if(  ans[0] < 0.0f && ans[1] < 0.0f )
		{
			VecAfterToBefore = pos_before - pos_after;

			//	���Ε����Ƀ��C���΂�
			D3DXVec3Normalize (&_direction ,&VecAfterToBefore);
			//	�ړ��������C�Ƃ̓����蔻����s��
			D3DXIntersect (mesh,&pos_after,&_direction,&hit,&index,nullptr,nullptr,&dist,nullptr,nullptr );
			
			if( hit )
			{
				if( resultpos )
				{
					*resultpos = pos_after + _direction*(dist+0.05f);
					//	�����蔻��I�u�W�F�N�g�̃��[�J�����W�n�ł̈ʒu�Ȃ̂ŁA��΍��W�ɕϊ����Ȃ���
					D3DXVec3TransformCoord(resultpos,resultpos,&matrix);
				}
				return true;	
			}

		}
	}
	
	//	�����܂ł����瓖�����Ă��Ȃ��̂�false��Ԃ�
	return false;
}

//	���C�s�b�L���O�ɂ���������@������W���Î~���Ă���A���b�V���̈ړ��O�A�ړ���̍s����ł̔���(���C�͓������ɂ܂�_�ɂȂ���Ă���,�����ƃ��b�V���������Ă���)
bool RayPickToMesh::RaypicktoMesh( bool moveflag , D3DXVECTOR3 *resultpos, D3DXVECTOR3 pos , LPD3DXMESH mesh , D3DXMATRIX matrix_befoe , D3DXMATRIX matrix_afiter )
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//	�|���S���ɂ������Ă��ׂ鏈��
	//
	D3DXVECTOR3 pos_before,pos_after;
	D3DXMATRIX matrix = matrix_afiter;

	//	��������F�I�u�W�F�N�g�̃��[�J�����W�n�œ����蔻����s�����߁A
	//	��΍��W�̈ʒu�����������������I�u�W�F�N�g��ϊ������s��̋t�s����g���Ă���
	BOOL hit;DWORD index;float dist;					//	����������󂯎��ϐ�
	D3DXVECTOR3 _direction;				//	�ϊ���̈ʒu�A�������i�[����ϐ��F�{���͂����Œ�`���ׂ��\���p�ɃO���[�o����`�ɂ��Ă���
	D3DXMATRIX invmat_b,invmat_a;									//	�t�s����i�[����ϐ�
	D3DXVECTOR3 vertex[3],vertexnormal[3];				//�@1�|���S�����\������3���_�̍��W�ƕ���
	D3DXVECTOR3 polyNormal;								//	�|���S���̖@���i�����j
	D3DXPLANE	plane;									//	���ʏ����p�̍\���́F�|���S���𕽖ʂƂ��ď������邽��

	D3DXMatrixInverse ( &invmat_b , nullptr , &matrix_befoe );	//	�t�s��̎擾
	D3DXMatrixInverse ( &invmat_a , nullptr , &matrix_afiter );	
	
	//	���b�V�����猩���O�t���[���̈ʒu�ƌ��݃t���[���̈ʒu���Z�o

	D3DXVec3TransformCoord(&pos_before,&pos,&invmat_b);
	D3DXVec3TransformCoord(&pos_after,&pos,&invmat_a);

	//D3DXVec3TransformCoord(&pos_before,&pos_before,&invmat);	//	�t�s����g�p���A���C�n�_����ϊ��@�ʒu�ƌ����ŕϊ�����֐����قȂ�̂ŗv����
	//D3DXVec3TransformCoord(&pos_after,&pos_after,&invmat);		//	�t�s����g�p���A���C�I�_����ϊ��@�ʒu�ƌ����ŕϊ�����֐����قȂ�̂ŗv����

	D3DXVECTOR3 VecAfterToBefore = pos_after - pos_before;
	D3DXVec3Normalize (&_direction ,&VecAfterToBefore);				//	�t�s����g�p���A���C��������ϊ��@�ʒu�ƌ����ŕϊ�����֐����قȂ�̂ŗv����
	
	//	�ړ��������C�Ƃ̓����蔻����s��
	D3DXIntersect (mesh,&pos_before,&_direction,&hit,&index,nullptr,nullptr,&dist,nullptr,nullptr );

	//	����͓����蔻��̃`�F�b�N�ł͂Ȃ��_�ɒ��ӁF�����܂ňړ������Ƀ|���S�������邩�ǂ����𔻒f���Ă���
	if( hit )
	{

		//	�ړ������̐�ɂ́A���̔ԍ��̃|���S���ɓ�����炵��
		int polyindex = index;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		//	���b�V���Ǝ擾���A���_�t�H�[�}�b�g�Ȃǂ̏����擾
		//	���b�V������A�����蔻��Ɏg�p���邱�ƂɂȂ�|���S���̂̏����擾����
		//	���̂�����̏����͂قڂ���Œ�^�Ǝv���Ă���
		//
		LPD3DXMESH pMesh = mesh;			//	���b�V���N���X��������b�V�����擾
		DWORD fvf = pMesh->GetFVF();							//	�����b�V�����璸�_�����擾

		//	�C���f�b�N�X�o�b�t�@�ƒ��_�o�b�t�@�̎擾
		//	�C���f�b�N�X�o�b�t�@�Ƃ�1�|���S�����\������̂ɂǂ̃|���S�������悤����Ă��邩��\���F�ʏ�3�_��1�|�����\������
		//LPDIRECT3DINDEXBUFFER9	pI;								//	�C���f�b�N�X�o�b�t�@
		//D3DINDEXBUFFER_DESC		Idesc;							//	�C���f�b�N�X�o�b�t�@�̍\�����
		LPVOID pVB,pIB;											//	�`���Ȃ��|�C���^�A�܂����̌`���Ń|�C���^���擾����
		_XFileIndexBuffer *i_pt;								//	������������C���f�b�N�X�o�b�t�@�����p�\���̂ւ̃|�C���^
		

//		pMesh->GetIndexBuffer( &pI );							//	�C���f�b�N�X�o�b�t�@���擾	
//		pI->GetDesc( &Idesc );									//	�C���f�b�N�X�o�b�t�@����\�������擾
		pMesh->LockIndexBuffer( D3DLOCK_READONLY , &pIB );		//	�C���f�b�N�X�o�b�t�@�����b�N���|�C���^���擾
		pMesh->LockVertexBuffer( D3DLOCK_READONLY , &pVB );		//	�o�[�e�b�N�X�o�b�t�@�����b�N���|�C���^���擾
		pMesh->UnlockIndexBuffer();								//	���b�N���ėp�����񂾂炷���A�����b�N
		pMesh->UnlockVertexBuffer();							//	���b�N���ėp�����񂾂炷���A�����b�N
		
		//	�C���f�b�N�X�o�b�t�@��l�擾�p�\���̂ɃL���X�g
		i_pt = (_XFileIndexBuffer*)pIB;							//	�������邱�Ƃɂ��A�����̃������̈���������K�肵���f�[�^�`���ŏ����ł���A���ꂪC����̈�ԋ��͂ȋ@�\�ł���
		//	�K�v�ȃC���f�b�N�X�܂Ői�߂�
		for( int i=0 ; i<polyindex ; i++ )
		{
			i_pt++;
		}
		
		//	���_�t�H�[�}�b�g�ɂ���ăL���X�g����\���̂�ύX����
		if( fvf == ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 ) )		//	�ʒu�F�����F�e�N�X�`��1���̏ꍇ
		{
			//	���`�|�C���^�������̍쐬�����o�[�e�b�N�X�o�b�t�@�����p�̃|�C���^�ɃL���X�g����
			//	�������邱�Ƃɂ��A�����̃������̈���������K�肵���f�[�^�`���ŏ����ł���A���ꂪC����̈�ԋ��͂ȋ@�\�ł���	
			_XFileVertexBuffer_p_n_t *v_pt;								//	�ʒu�F�����F�e�N�X�`��1���̍\���̂̃|�C���^			
			v_pt = (_XFileVertexBuffer_p_n_t*)pVB;						//	�L���X�g

			//	3���_�̃f�[�^���擾����
			for( int i=0 ; i<3 ; i++ )
			{
				//	���_�o�b�t�@�̐擪�A�h���X�擾
				_XFileVertexBuffer_p_n_t *w_pt = v_pt;

				//	�擾�������_�ԍ��܂Ń|�C���^���X�L�b�v
				for( int j=0 ; j<i_pt->p[i] ; j++ ) w_pt++;

				//	�Y�����钸�_�̖@�����擾�ƒ��_�ʒu�����擾
				vertexnormal[i] = w_pt->normal;							//	�������
				vertex[i] = w_pt->pos;									//	�ʒu���
			}
		}
		if( fvf == ( D3DFVF_XYZ | D3DFVF_NORMAL) )						//	�ʒu�F�����̏ꍇ
		{
			//	���`�|�C���^�������̍쐬�����o�[�e�b�N�X�o�b�t�@�����p�̃|�C���^�ɃL���X�g����
			//	�������邱�Ƃɂ��A�����̃������̈���������K�肵���f�[�^�`���ŏ����ł���A���ꂪC����̈�ԋ��͂ȋ@�\�ł���	
			_XFileVertexBuffer_p_n *v_pt;								//	�ʒu�F�����\���̂̃|�C���^			
			v_pt = (_XFileVertexBuffer_p_n*)pVB;						//	�L���X�g

			//	3���_�̃f�[�^���擾����
			for( int i=0 ; i<3 ; i++ )
			{
				//	���_�o�b�t�@�̐擪�A�h���X�擾
				_XFileVertexBuffer_p_n *w_pt = v_pt;

				//	�擾�������_�ԍ��܂Ń|�C���^���X�L�b�v
				for( int j=0 ; j<i_pt->p[i] ; j++ ) w_pt++;

				//	�Y�����钸�_�̖@�����擾�ƒ��_�ʒu�����擾
				vertexnormal[i] = w_pt->normal;							//	�������
				vertex[i] = w_pt->pos;									//	�ʒu���
			}
		}
		
		//	�擾����3�_�̒��_�f�[�^����A�|���S���̌��������߁A�������炷�ׂ��̍��W�����߂�
		//	�擾����3���_�̃f�[�^����|���S���̖@�������߂�
		polyNormal.x = ( vertexnormal[0].x + vertexnormal[1].x + vertexnormal[2].x)/3;
		polyNormal.y = ( vertexnormal[0].y + vertexnormal[1].y + vertexnormal[2].y)/3;
		polyNormal.z = ( vertexnormal[0].z + vertexnormal[1].z + vertexnormal[2].z)/3;
		
		//	3�_��p���Ė������ʂ��`����
		D3DXPlaneFromPoints( &plane,&vertex[0],&vertex[1],&vertex[2]);

		


		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		//	�ړ��O��̍��W���|���S��������ł��邩���肵�A�͂���ł���΃A�^�b�g�~�Ȃ��A���ׂ菈�����s��
		//	���̂����肩��͎��ۂ̈ړ��O��̍��W���g�p����̂Œ��ӂ���

		//	�ړ���A�O�̈ʒu�����ʂ��͂��ނ��`�F�b�N
		float ans[2];	//	���w�̕��ʂƒ��_�Ɋւ�������𗘗p����F���ʂ̕\�ʂɂ���Ȃ琳�F���ʂȂ畉�ɂȂ�
		ans[0] =	( plane.a * pos_after.x ) +(	plane.b * pos_after.y )+(plane.c * pos_after.z )+plane.d;
		ans[1] =	( plane.a * pos_before.x ) +(	plane.b * pos_before.y )+(plane.c * pos_before.z )+plane.d;

		//	��̒l�̐��������΂Ȃ�΁A���ʂ��͂���->�������Ă���̂ł��ׂ菈�����s��
		if(  ans[0]*ans[1] < 0.0f )
		{
			//	�q�b�g���Ă����ꍇ�ɂ��ׂ胂�[�h���L���łȂ���΁A�Փˈʒu��Ԃ��ďI��
			if( moveflag == false )
			{
				if( resultpos )
				{
					*resultpos = pos_before + _direction*(dist+0.05f);
					//	�����蔻��I�u�W�F�N�g�̃��[�J�����W�n�ł̈ʒu�Ȃ̂ŁA��΍��W�ɕϊ����Ȃ���
					D3DXVec3TransformCoord(resultpos,resultpos,&matrix);
				}
			}
			else
			{
				//	���ׂ��̍��W������ϐ�
				D3DXVECTOR3 afterpos;
				//	�ړ���̍��W����|���S���@�������Ƀ��C���΂��A�|���S���Ƃ̌�_�����߂�
				D3DXIntersect (mesh,&pos_after,&polyNormal,&hit,&index,nullptr,nullptr,&dist,nullptr,nullptr );
				//	�������Ă����ꍇ�́A���C�\�����̏I�_�ʒu���X�V����
				if( hit )
				{
					//	���ׂ��̍��W�|���S���̕\�ʂ�菭�����߂ɖ߂�
					afterpos = 	pos_after+polyNormal*(dist+0.05f);
					
					//	�����蔻��I�u�W�F�N�g�̃��[�J�����W�n�ł̈ʒu�Ȃ̂ŁA��΍��W�ɕϊ����Ȃ���
					D3DXVec3TransformCoord(&afterpos,&afterpos,&matrix);
					
					//	���ׂĂ̏��������񂾍��W���Z�b�g
					if( resultpos )
					{
						*resultpos = afterpos;
					}
				}
			}
			return true;	
		}

		//	2�̒l�����̏ꍇ�A�I�u�W�F�N�g�ɖ��܂��Ă��܂��Ă���̂ŁA���Ε����̒��߃|���S���܂ňړ�
		if(  ans[0] < 0.0f && ans[1] < 0.0f )
		{
			VecAfterToBefore = pos_before - pos_after;

			//	���Ε����Ƀ��C���΂�
			D3DXVec3Normalize (&_direction ,&VecAfterToBefore);
			//	�ړ��������C�Ƃ̓����蔻����s��
			D3DXIntersect (mesh,&pos_after,&_direction,&hit,&index,nullptr,nullptr,&dist,nullptr,nullptr );
			
			if( hit )
			{
				if( resultpos )
				{
					*resultpos = pos_after + _direction*(dist+0.05f);
					//	�����蔻��I�u�W�F�N�g�̃��[�J�����W�n�ł̈ʒu�Ȃ̂ŁA��΍��W�ɕϊ����Ȃ���
					D3DXVec3TransformCoord(resultpos,resultpos,&matrix);
				}
				return true;	
			}

		}
	}
	
	//	�����܂ł����瓖�����Ă��Ȃ��̂�false��Ԃ�
	return false;
}










