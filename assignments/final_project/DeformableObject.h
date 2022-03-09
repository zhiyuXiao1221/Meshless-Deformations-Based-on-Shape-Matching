//////////////////////////////////////////////////////////////////////////
//// Dartmouth Physical Computing Final Project 1: MeshlessDeformations
//// Author: Zhiyu Xiao
////////////////////////////////////////////////////////////////////////// 

#ifndef __DeformableObject_h__
#define __DeformableObject_h__
#include "Common.h"
#include "Particles.h"

#include <unsupported/Eigen/MatrixFunctions>

class DeformableObject
{
public:
	////Spring parameters
	Particles<3> particles;								//// The particle system. Each particle has the attributes of position, velocity, mass, and force. Read Particles.h in src to know the details.
	std::vector<Vector2i> springs;						//// Each element in springs stores a Vector2i for the indices of the two end points.
	double alpha = .999;
    double beta = 0.9;
    double elasticity = 1.0;
    double kd=(double).2;		////damping for the collision force
	////Boundary nodes
	std::unordered_map<int,Vector3> boundary_nodes;		//// boundary_notes stores the mapping from node index to its specified velocity. E.g., a fixed node will have a zero velocity.

	////Body force
	Vector3 g=Vector3::Unit(1)*(double)-1.;			//// gravity

    Vector3 t_0 = Vector3::Unit(0) * (double)1.0; //original center mass
    Vector3 t = Vector3::Unit(0) * (double)1.0;// current center mass
    std::vector<Vector3> q;
    std::vector<Vector3> p;
    std::vector<Vector3> current_positions;
    std::vector<Vector3> goal_positions;
    Matrix3 Aqq;
    Matrix3 Aqq_inverse;
    Matrix3 Apq;
    Matrix3 R;
    Matrix3 S;
    std::vector<Vector3> my_object_vertices={{-3.,-1.,0.0},{3.,-1.,0.0}};	////this array stores the positions of the contour of your object for visualization


	virtual void Initialize()
	{
        InitialCalculations(); //initialize q, Aqq
	}

	virtual void Advance(const double dt)
	{
		//// Step 0: Clear the force on each particle (already done for you)
		Clear_Force();
		//// Step 1: add a body force and drag force to each particle
		Apply_Body_Force_and_Drag_Force();
        Update_t_p_and_Apq(dt);
        Update_RotationMatrix();
        Update_GoalPositions();
        Update_ParticlePositions(dt);

	}
	
	////Set boundary nodes
	void Set_Boundary_Node(const int p,const Vector3 v=Vector3::Zero()){boundary_nodes[p]=v;}
	
	bool Is_Boundary_Node(const int p){return boundary_nodes.find(p)!=boundary_nodes.end();}


	void Clear_Force()
	{
		for(int i=0;i<particles.Size();i++){particles.F(i)=Vector3::Zero();}
	}

	void Apply_Body_Force_and_Drag_Force()
	{
		/* Your implementation start */
		for(int i = 0; i < particles.Size(); i++){
			particles.F(i) += g * particles.M(i);
            //particles.F(i) +=  kd * -1.0 * particles.V(i) ;
		}

		/* Your implementation end */	
	}

	void Enforce_Boundary_Condition()
	{
		/* Your implementation start */
		for(auto& particle: boundary_nodes){
			particles.V(particle.first) = particle.second;
			particles.F(particle.first) = Vector3::Zero();
		}
		/* Your implementation end */	
	}

 
	void Time_Integration(const double dt)
	{
		/* Your implementation start */
		for(int i = 0; i< particles.Size(); i++){
			particles.V(i) += particles.F(i) / particles.M(i) * dt;
			particles.X(i) += particles.V(i) * dt;
		}

		/* Your implementation end */		
	}
    bool Check_Ground_Collide(Vector3 pos,double raduis)
    {
        // if (pos[1]-raduis<-1.)
        // {return true;}
        double signed_distance = (pos[1] -(-1.0));
		if(signed_distance - raduis < 0){  //colide with ground
            return true;
		}
        return false;
    }

    void InitialCalculations()
    {
        //resize q and p
        q.resize(particles.Size());
        p.resize(particles.Size());
        goal_positions.resize(particles.Size());
        current_positions.resize(particles.Size());
        //calculate t_0 
        double sum_mass = 0.0;
        Vector3 sum_position = Vector3::Zero();
        for(int i = 0; i<particles.Size();i++)
        {
            sum_mass += particles.M(i);
            sum_position += particles.M(i) * particles.X(i);
        }
        t_0 = sum_position / sum_mass;
        
        //calculate q, Aqq
        for(int i = 0; i<particles.Size();i++)
        {
            q[i] = particles.X(i) - t_0;
            Aqq_inverse += particles.M(i) * q[i] * q[i].transpose();
        }
        Aqq = Aqq_inverse.llt().solve(Matrix3::Identity());
    }


    // void Update_t_p_and_Apq(const double dt)
    // {
    //     // update t : t is only based on external forces
    //     double sum_mass = 0.0;
    //     Vector3 sum_position = Vector3::Zero();
    //     for(int i = 0; i< particles.Size(); i++){
	// 		Vector3 tmp_v = particles.V(i) + particles.F(i) / particles.M(i) * dt;
	// 		Vector3 tmp_x = particles.X(i) + tmp_v * dt;
    //         sum_position += particles.M(i) * tmp_x;
    //         current_positions[i] = tmp_x;
    //         sum_mass += particles.M(i);
	// 	}
    //     t = sum_position / sum_mass;
        
    //     //update p Apq
    //      for(int i = 0; i<particles.Size();i++)
    //     {
    //         p[i] = current_positions[i] - t;
    //         Apq += particles.M(i) * p[i] * q[i].transpose();
    //     }
    // }

    void Update_t_p_and_Apq(const double dt)
    {
        // update t : t is only based on external forces
        double sum_mass = 0.0;
        Vector3 sum_position = Vector3::Zero();
        for(int i = 0; i< particles.Size(); i++){
			Vector3 tmp_x = particles.X(i);
            sum_position += particles.M(i) * tmp_x;
            sum_mass += particles.M(i);
		}
        t = sum_position / sum_mass;
        
        //update p Apq
        Apq *= 0.;
        for(int i = 0; i<particles.Size();i++)
        {
            p[i] = particles.X(i) - t;
            Apq += particles.M(i) * p[i] * q[i].transpose();
        }
    }

    void Update_RotationMatrix()  //R
    {  
        S = (Apq.transpose() * Apq).sqrt();
        R = Apq * S.llt().solve(Matrix3::Identity());
    }
    void Update_GoalPositions()
    {
         for(int i = 0; i<particles.Size();i++)
        {
            goal_positions[i] = R * q[i] + t;
        }
    }
    void Update_ParticlePositions(const double dt)
    {
        for(int i = 0; i<particles.Size();i++)
        {
            //particles.X(i) = goal_positions[i];
            //continue;
            particles.V(i) += alpha * (goal_positions[i]-particles.X(i))/dt;
            particles.V(i) += particles.F(i)/particles.M(i) * dt;
            Vector3 translation = particles.V(i) * dt;
            if(Check_Ground_Collide(particles.X(i)+translation,particles.R(i)))
            //if (Check_Ground_Collide(particles.X(i)+translation, 0.))
            {
                //particles.V(i) *= -1.0;
                particles.V(i) += 0.3 * 1./dt * (-1.-((particles.X(i)+translation)[1]-particles.R(i))) * Vector3::Unit(1);
                particles.X(i) += particles.V(i) * dt;
            }else
            {
                //particles.V(i) += alpha * (goal_positions[i]-current_positions[i])/dt + dt * particles.F(i)/particles.M(i);
                particles.X(i) += translation;

            }
        }
    }



	////Hint: you may want to use these functions when assembling your implicit matrix
	////Add block nonzeros to sparse matrix elements (for initialization)
	void Add_Block_Triplet_Helper(const int i,const int j,std::vector<TripletT>& elements)
	{for(int ii=0;ii<3;ii++)for(int jj=0;jj<3;jj++)elements.push_back(TripletT(i*3+ii,j*3+jj,(double)0));}

	////Add block Ks to K_ij
	void Add_Block_Helper(SparseMatrixT& K,const int i,const int j,const Matrix3& Ks)
	{
		SparseFunc::Add_Block<3,Matrix3>(K,i,i,Ks);
		SparseFunc::Add_Block<3,Matrix3>(K,j,j,Ks);
		if(!Is_Boundary_Node(i)&&!Is_Boundary_Node(j)){
			SparseFunc::Add_Block<3,Matrix3>(K,i,j,-Ks);
			SparseFunc::Add_Block<3,Matrix3>(K,j,i,-Ks);}
	}

	////Set block values on a vector
	void Set_Block(VectorX& b,const int i,const Vector3& bi)
	{for(int ii=0;ii<3;ii++)b[i*3+ii]=bi[ii];}

	////Add block values to a vector
	void Add_Block(VectorX& b,const int i,const Vector3& bi)
	{for(int ii=0;ii<3;ii++)b[i*3+ii]+=bi[ii];}
};

#endif