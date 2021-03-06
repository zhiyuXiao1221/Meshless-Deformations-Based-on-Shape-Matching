//////////////////////////////////////////////////////////////////////////
//// Dartmouth Physical Computing Programming Assignment 2: DEM Particle Sand
//// Author: TODO: PUT YOUR NAME HERE
////////////////////////////////////////////////////////////////////////// 

#ifndef __ParticleDeformable_h__
#define __ParticleDeformable_h__
#include "Common.h"
#include "Particles.h"
#include "ImplicitGeometry.h"

#include <unsupported/Eigen/MatrixFunctions>

template<int d> class ParticleDeformable
{using VectorD=Vector<double,d>;using VectorDi=Vector<int,d>;using MatrixD=Matrix<double,d>;
public:
	bool just_released = false;
	bool dragging = false;
	VectorD handle_sphere_pos = VectorD::Zero();
	VectorD init_handle_sphere_pos = VectorD::Zero();
	int handle_sphere_idx = -1;
	double weight = 1.0;
	double handle_sphere_influenced_radius = 0.;
	std::vector<int> handle_sphere_influenced_idx;
	std::vector<double> handle_sphere_influenced_dist;
	double handle_sphere_r = 0.;
	int test = 1;
	double dx = 1.;
	double total_mass = 0.;
	std::vector<int> fixed;
	std::vector<double> phis;
	std::vector<VectorD> init_positions;
	std::vector<VectorD> qs;
	std::vector<Vector<double, 9>> qs_tilde;
	VectorD init_COM = VectorD::Zero();
	VectorD curr_COM = VectorD::Zero();
	Matrix3 Aqq = Matrix3::Zero();
	Eigen::Matrix<double, 9, 9> Aqq_tilde = Eigen::Matrix<double, 9, 9>::Zero();
	Particles<d> particles;
	VectorD g=VectorD::Unit(1)*(double)-1.;	////gravity

	//plasticity related params
	Matrix3 Sp = Matrix3::Identity();
	double c_creep = 10.;
	double c_yield = 0.1;
	double c_max = 0.5;
	//params
	double alpha = .5;
	double beta = .8;
	double decay = 1.;
	double alpha_cluster = .4;


	////a list of implicit geometries describing the environment, by default it has one element, a circle with its normals pointing inward (Bowl)
	std::vector<ImplicitGeometry<d>* > env_objects;	
	//springs if use mesh format
	std::vector<Vector2i> springs;
	//cluster part
	bool useCluster = false;
	std::vector<int> cluster_one_index;
	std::vector<int> cluster_two_index;
	//cluster one
	Particles<d> cluster_one;
	double total_mass_cluster_one = 0.0;
	std::vector<VectorD> init_positions_cluster_one;
	std::vector<VectorD> qs_cluster_one;
	std::vector<Vector<double, 9>> qs_tilde_cluster_one;
	VectorD init_COM_cluster_one = VectorD::Zero();
	VectorD curr_COM_cluster_one = VectorD::Zero();
	Matrix3 Aqq_cluster_one = Matrix3::Zero();
	Eigen::Matrix<double, 9, 9> Aqq_tilde_cluster_one = Eigen::Matrix<double, 9, 9>::Zero();

	//cluster two
	Particles<d> cluster_two;
	double total_mass_cluster_two = 0.0;
	std::vector<VectorD> init_positions_cluster_two;
	std::vector<VectorD> qs_cluster_two;
	std::vector<Vector<double, 9>> qs_tilde_cluster_two;
	VectorD init_COM_cluster_two = VectorD::Zero();
	VectorD curr_COM_cluster_two = VectorD::Zero();
	Matrix3 Aqq_cluster_two = Matrix3::Zero();
	Eigen::Matrix<double, 9, 9> Aqq_tilde_cluster_two = Eigen::Matrix<double, 9, 9>::Zero();

	void Initialize() {
		for (int i = 0; i < particles.Size(); i++) {
			init_positions.push_back(particles.X(i));
			init_COM += particles.M(i) * particles.X(i);
			total_mass += particles.M(i);
		}
		init_COM /= total_mass;
		// Record the qs
		for (int i = 0; i < particles.Size(); i++) {
			qs.push_back(particles.X(i)-init_COM);
		}
		// Compute Aqq and Aqq_tilde
		for (int i = 0; i < particles.Size(); i++) {
			VectorD qi = qs[i];
			//Aqq
			Aqq += particles.M(i) * qi * qi.transpose();
			//Aqq tilde
			Vector<double, 9> qi_tilde;
			qi_tilde << qi(0), qi(1), qi(2),
				qi(0)* qi(0), qi(1)* qi(1), qi(2)* qi(2),
				qi(0)* qi(1), qi(1)* qi(2), qi(2)* qi(0);
			qs_tilde.push_back(qi_tilde);
			Aqq_tilde += particles.M(i) * qi_tilde * qi_tilde.transpose();
		}
		//its important that we do this instead of A.inverse(), since direct solving will behave bad when we have entries like 1.e-15
		Aqq = Aqq.llt().solve(Matrix3::Identity()); // LLT assumes that Aqq is symmetric positive definite
		Aqq_tilde = Aqq_tilde.llt().solve(Eigen::Matrix<double, 9, 9>::Identity());

		phis.resize(particles.Size());

		if(useCluster) {
			divide_Two_Clusters();
			cluster_one_Initialize();
			cluster_two_Initialize();
		}

	}

	void divide_Two_Clusters() {
		for (int i = 0; i < particles.Size(); i++) {
			if(particles.X(i)[1] <=6.0) {
				int k = cluster_one.Add_Element();
				cluster_one.X(k) = particles.X(i);
				cluster_one.V(k) = particles.V(i);
				cluster_one.M(k) = particles.M(i);
				cluster_one.R(k) = particles.R(i);
				cluster_one_index.push_back(i);
			}else {
				int k = cluster_two.Add_Element();
				cluster_two.X(k) = particles.X(i);
				cluster_two.V(k) = particles.V(i);
				cluster_two.M(k) = particles.M(i);
				cluster_two.R(k) = particles.R(i);
				cluster_two_index.push_back(i);
			}
		}
	}
	void cluster_one_Initialize() {
			for (int i = 0; i < cluster_one.Size(); i++) {
			init_positions_cluster_one.push_back(cluster_one.X(i));
			init_COM_cluster_one += cluster_one.M(i) * cluster_one.X(i);
			total_mass_cluster_one += cluster_one.M(i);
		}
		init_COM_cluster_one /= total_mass_cluster_one;
		// Record the qs
		for (int i = 0; i < cluster_one.Size(); i++) {
			qs_cluster_one.push_back(cluster_one.X(i)-init_COM_cluster_one);
		}
		// Compute Aqq and Aqq_tilde
		for (int i = 0; i < cluster_one.Size(); i++) {
			VectorD qi = qs_cluster_one[i];
			//Aqq
			Aqq_cluster_one += cluster_one.M(i) * qi * qi.transpose();
			//Aqq tilde
			Vector<double, 9> qi_tilde;
			qi_tilde << qi(0), qi(1), qi(2),
				qi(0)* qi(0), qi(1)* qi(1), qi(2)* qi(2),
				qi(0)* qi(1), qi(1)* qi(2), qi(2)* qi(0);
			qs_tilde_cluster_one.push_back(qi_tilde);
			Aqq_tilde_cluster_one += cluster_one.M(i) * qi_tilde * qi_tilde.transpose();
		}
		//its important that we do this instead of A.inverse(), since direct solving will behave bad when we have entries like 1.e-15
		Aqq_cluster_one = Aqq_cluster_one.llt().solve(Matrix3::Identity()); // LLT assumes that Aqq is symmetric positive definite
		Aqq_tilde_cluster_one = Aqq_tilde_cluster_one.llt().solve(Eigen::Matrix<double, 9, 9>::Identity());
	}
	// modify qs, qs_tilde, Aqq, Aqq_tilde to reflect plasticity

	void cluster_two_Initialize() {
			for (int i = 0; i < cluster_two.Size(); i++) {
			init_positions_cluster_two.push_back(cluster_two.X(i));
			init_COM_cluster_two += cluster_two.M(i) * cluster_two.X(i);
			total_mass_cluster_two += cluster_two.M(i);
		}
		init_COM_cluster_two /= total_mass_cluster_two;
		// Record the qs
		for (int i = 0; i < cluster_two.Size(); i++) {
			qs_cluster_two.push_back(cluster_two.X(i)-init_COM_cluster_two);
		}
		// Compute Aqq and Aqq_tilde
		for (int i = 0; i < cluster_two.Size(); i++) {
			VectorD qi = qs_cluster_two[i];
			//Aqq
			Aqq_cluster_two += cluster_two.M(i) * qi * qi.transpose();
			//Aqq tilde
			Vector<double, 9> qi_tilde;
			qi_tilde << qi(0), qi(1), qi(2),
				qi(0)* qi(0), qi(1)* qi(1), qi(2)* qi(2),
				qi(0)* qi(1), qi(1)* qi(2), qi(2)* qi(0);
			qs_tilde_cluster_two.push_back(qi_tilde);
			Aqq_tilde_cluster_two += cluster_two.M(i) * qi_tilde * qi_tilde.transpose();
		}
		//its important that we do this instead of A.inverse(), since direct solving will behave bad when we have entries like 1.e-15
		Aqq_cluster_two = Aqq_cluster_two.llt().solve(Matrix3::Identity()); // LLT assumes that Aqq is symmetric positive definite
		Aqq_tilde_cluster_two = Aqq_tilde_cluster_two.llt().solve(Eigen::Matrix<double, 9, 9>::Identity());
	}
	void Register_Plasticity() {
		// RE-Record the qs
		qs.clear();
		qs_tilde.clear();
		Aqq *= 0.;
		Aqq_tilde *= 0.;
		for (int i = 0; i < particles.Size(); i++) {
			qs.push_back(Sp * (init_positions[i] - init_COM));
		}
		// Compute Aqq and Aqq_tilde
		for (int i = 0; i < particles.Size(); i++) {
			VectorD qi = qs[i];
			//Aqq
			Aqq += particles.M(i) * qi * qi.transpose();
			//Aqq tilde
			Vector<double, 9> qi_tilde;
			qi_tilde << qi(0), qi(1), qi(2),
				qi(0)* qi(0), qi(1)* qi(1), qi(2)* qi(2),
				qi(0)* qi(1), qi(1)* qi(2), qi(2)* qi(0);
			qs_tilde.push_back(qi_tilde);
			Aqq_tilde += particles.M(i) * qi_tilde * qi_tilde.transpose();
		}
		Aqq = Aqq.llt().solve(Matrix3::Identity());
		Aqq_tilde = Aqq_tilde.llt().solve(Eigen::Matrix<double, 9, 9>::Identity());
	}

	void Shape_Match_Basic(const double dt) {
		////Compute the force term that corresponds to g - x / h in the paper
		// update curr COM
		curr_COM = VectorD::Zero();
		for (int i = 0; i < particles.Size(); i++) {
			curr_COM += particles.M(i) * particles.X(i);
		}
		curr_COM /= total_mass;
		// compute g
		Matrix3 Apq = Matrix3::Zero();
		for (int i = 0; i < particles.Size(); i++) {
			VectorD qi = qs[i];
			VectorD pi = particles.X(i) - curr_COM;
			Apq += particles.M(i) * pi * qi.transpose();
		}
		Matrix3 S = (Apq.transpose() * Apq);
		S = S.sqrt();
		Matrix3 R = Apq * S.llt().solve(Matrix3::Identity());

		for (int i = 0; i < particles.Size(); i++) {
			VectorD gi = R * (qs[i]) + curr_COM;
			particles.V(i) += alpha * 1. / dt * (gi - particles.X(i));
		}
	}

	void Shape_Match_Linear(const double dt) {
		////Compute the force term that corresponds to g - x / h in the paper
		// update curr COM
		curr_COM = VectorD::Zero();
		for (int i = 0; i < particles.Size(); i++) {
			curr_COM += particles.M(i) * particles.X(i);
		}
		curr_COM /= total_mass;
		// compute g
		Matrix3 Apq = Matrix3::Zero();
		for (int i = 0; i < particles.Size(); i++) {
			VectorD qi = qs[i];
			VectorD pi = particles.X(i) - curr_COM;
			Apq += particles.M(i) * pi * qi.transpose();
		}
		Matrix3 S = (Apq.transpose() * Apq);
		S = S.sqrt();
		Matrix3 R = Apq * S.llt().solve(Matrix3::Identity());
		Matrix3 A = Apq * Aqq;

		A /= cbrt(A.determinant());
		for (int i = 0; i < particles.Size(); i++) {
			VectorD gi = (beta * A + (1-beta) * R) * (qs[i]) + curr_COM;
			particles.V(i) += alpha * 1. / dt * (gi - particles.X(i));
		}
	}

	void Shape_Match_Quadratic(const double dt) {
		////Compute the force term that corresponds to g - x / h in the paper
		// update curr COM
		curr_COM = VectorD::Zero();
		for (int i = 0; i < particles.Size(); i++) {
			curr_COM += particles.M(i) * particles.X(i);
		}
		curr_COM /= total_mass;
		// compute g
		Matrix3 Apq = Matrix3::Zero();
		for (int i = 0; i < particles.Size(); i++) {
			VectorD qi = qs[i];
			VectorD pi = particles.X(i) - curr_COM;
			Apq += particles.M(i) * pi * qi.transpose();
		}
		Matrix3 S = (Apq.transpose() * Apq);
		S = S.sqrt();
		Matrix3 R = Apq * S.llt().solve(Matrix3::Identity());
		Eigen::Matrix<double, 3, 9> R_tilde = Eigen::Matrix<double, 3, 9>::Zero();
		R_tilde.block<3, 3>(0, 0) = R;
		//Compute A_tilde via quadratic
		Eigen::Matrix<double, 3, 9> Apq_tilde = Eigen::Matrix<double, 3, 9>::Zero();
		for (int i = 0; i < particles.Size(); i++) {
			Vector<double, 9> qi_tilde = qs_tilde[i];
			VectorD pi = particles.X(i) - curr_COM;
			Apq_tilde += particles.M(i) * pi * qi_tilde.transpose();
		}
		Eigen::Matrix<double, 3, 9> A_tilde = Apq_tilde * Aqq_tilde;

		for (int i = 0; i < particles.Size(); i++) {
			VectorD gi = (beta * A_tilde + (1 - beta) * R_tilde) * (qs_tilde[i]) + curr_COM;
			particles.V(i) += alpha * 1. / dt * (gi - particles.X(i));
		}
	}

	void Shape_Match_Quadratic_Plasticity(const double dt) {
		////Compute the force term that corresponds to g - x / h in the paper
		// update curr COM
		curr_COM = VectorD::Zero();
		for (int i = 0; i < particles.Size(); i++) {
			curr_COM += particles.M(i) * particles.X(i);
		}
		curr_COM /= total_mass;
		// compute g
		Matrix3 Apq = Matrix3::Zero();
		for (int i = 0; i < particles.Size(); i++) {
			VectorD qi = qs[i];
			VectorD pi = particles.X(i) - curr_COM;
			Apq += particles.M(i) * pi * qi.transpose();
		}
		Matrix3 S = (Apq.transpose() * Apq);
		S = S.sqrt();

		Matrix3 A = Apq * Aqq;
		A /= cbrt(A.determinant());

		Matrix3 R = Apq * S.llt().solve(Matrix3::Identity());

		//plasticity-related
		Matrix3 SS = R.transpose() * A; // Note that this "S" in section 4.5 is not the same "S" in 3.3. 
										//This one has scaling accounted for by Aqq. In 3.3, A_pq = RS, here, A = A_pq * A_qq = RS.
		if ((SS - Matrix3::Identity()).norm() > c_yield) {
			Sp = (Matrix3::Identity() + dt * c_creep * (SS - Matrix3::Identity())) * Sp;
			if ((Sp - Matrix3::Identity()).norm() > c_max) {
				Sp = Matrix3::Identity() + c_max * (Sp - Matrix3::Identity()) / (Sp - Matrix3::Identity()).norm();
			}
			Sp /= cbrt(Sp.determinant());
			Register_Plasticity();
		}
		// done plasticity

		Eigen::Matrix<double, 3, 9> R_tilde = Eigen::Matrix<double, 3, 9>::Zero();
		R_tilde.block<3, 3>(0, 0) = R;
		//Compute A_tilde via quadratic
		Eigen::Matrix<double, 3, 9> Apq_tilde = Eigen::Matrix<double, 3, 9>::Zero();
		for (int i = 0; i < particles.Size(); i++) {
			Vector<double, 9> qi_tilde = qs_tilde[i];
			VectorD pi = particles.X(i) - curr_COM;
			Apq_tilde += particles.M(i) * pi * qi_tilde.transpose();
		}
		Eigen::Matrix<double, 3, 9> A_tilde = Apq_tilde * Aqq_tilde;

		for (int i = 0; i < particles.Size(); i++) {
			VectorD gi = (beta * A_tilde + (1 - beta) * R_tilde) * (qs_tilde[i]) + curr_COM;
			particles.V(i) += alpha * 1. / dt * (gi - particles.X(i));
		}
	}
	void One_Cluster_Shape_Match_Quadratic(const double dt ) {
		////Compute the force term that corresponds to g - x / h in the paper
		// update curr COM
		curr_COM_cluster_one = VectorD::Zero();
		for (int i = 0; i < cluster_one.Size(); i++) {
			curr_COM_cluster_one += cluster_one.M(i) * cluster_one.X(i);
		}
		curr_COM_cluster_one /= total_mass_cluster_one;
		// compute g
		Matrix3 Apq = Matrix3::Zero();
		for (int i = 0; i < cluster_one.Size(); i++) {
			VectorD qi = qs_cluster_one[i];
			VectorD pi = cluster_one.X(i) - curr_COM_cluster_one;
			Apq += cluster_one.M(i) * pi * qi.transpose();
		}
		Matrix3 S = (Apq.transpose() * Apq);
		S = S.sqrt();
		Matrix3 R = Apq * S.llt().solve(Matrix3::Identity());
		Eigen::Matrix<double, 3, 9> R_tilde = Eigen::Matrix<double, 3, 9>::Zero();
		R_tilde.block<3, 3>(0, 0) = R;
		//Compute A_tilde via quadratic
		Eigen::Matrix<double, 3, 9> Apq_tilde = Eigen::Matrix<double, 3, 9>::Zero();
		for (int i = 0; i < cluster_one.Size(); i++) {
			Vector<double, 9> qi_tilde = qs_tilde_cluster_one[i];
			VectorD pi = cluster_one.X(i) - curr_COM_cluster_one;
			Apq_tilde += cluster_one.M(i) * pi * qi_tilde.transpose();
		}
		Eigen::Matrix<double, 3, 9> A_tilde = Apq_tilde * Aqq_tilde_cluster_one;

		for (int i = 0; i < cluster_one.Size(); i++) {
			VectorD gi = (beta * A_tilde + (1 - beta) * R_tilde) * (qs_tilde_cluster_one[i]) + curr_COM_cluster_one;
			cluster_one.V(i) += alpha_cluster * 1. / dt * (gi - cluster_one.X(i));
		}
	}
	void Two_Cluster_Shape_Match_Quadratic(const double dt ) {
		////Compute the force term that corresponds to g - x / h in the paper
		// update curr COM
		curr_COM_cluster_two = VectorD::Zero();
		for (int i = 0; i < cluster_two.Size(); i++) {
			curr_COM_cluster_two += cluster_two.M(i) * cluster_two.X(i);
		}
		curr_COM_cluster_two /= total_mass_cluster_two;
		// compute g
		Matrix3 Apq = Matrix3::Zero();
		for (int i = 0; i < cluster_two.Size(); i++) {
			VectorD qi = qs_cluster_two[i];
			VectorD pi = cluster_two.X(i) - curr_COM_cluster_two;
			Apq += cluster_two.M(i) * pi * qi.transpose();
		}
		Matrix3 S = (Apq.transpose() * Apq);
		S = S.sqrt();
		Matrix3 R = Apq * S.llt().solve(Matrix3::Identity());
		Eigen::Matrix<double, 3, 9> R_tilde = Eigen::Matrix<double, 3, 9>::Zero();
		R_tilde.block<3, 3>(0, 0) = R;
		//Compute A_tilde via quadratic
		Eigen::Matrix<double, 3, 9> Apq_tilde = Eigen::Matrix<double, 3, 9>::Zero();
		for (int i = 0; i < cluster_two.Size(); i++) {
			Vector<double, 9> qi_tilde = qs_tilde_cluster_two[i];
			VectorD pi = cluster_two.X(i) - curr_COM_cluster_two;
			Apq_tilde += cluster_two.M(i) * pi * qi_tilde.transpose();
		}
		Eigen::Matrix<double, 3, 9> A_tilde = Apq_tilde * Aqq_tilde_cluster_two;

		for (int i = 0; i < cluster_two.Size(); i++) {
			VectorD gi = (beta * A_tilde + (1 - beta) * R_tilde) * (qs_tilde_cluster_two[i]) + curr_COM_cluster_two;
			cluster_two.V(i) += alpha_cluster * 1. / dt * (gi - cluster_two.X(i));
		}
	}
		void Clusters_Shape_Match_Quadratic(const double dt) {
		
		 	One_Cluster_Shape_Match_Quadratic(dt);
		 	Two_Cluster_Shape_Match_Quadratic(dt);
		// //Update particles.V

		for(int i = 0;i<cluster_one.Size();i++) {
			int index = cluster_one_index[i];
			VectorD diff = cluster_one.V(i) - particles.V(index);
			particles.V(index) += 0.75 * diff;
		}
		for(int i = 0; i< cluster_two.Size();i++) {
			int index = cluster_two_index[i];
			VectorD diff = cluster_one.V(i) - particles.V(index);
			particles.V(index) += 0.3 * diff;
		}
		
	}

	void Test_Specific_Vel_Operations() {
		if (test != 1) {

			//if (dragging) {
			//	for (int i = 0; i < particles.Size(); i++) {
			//		VectorD diff = handle_sphere_pos - particles.X(i);
			//		VectorD change = 20. * diff;
			//		decay = 1. / (1.5 * dx);
			//		change *= exp(-decay * diff.norm());
			//		particles.V(i) += change;
			//	}
			//}
			if (dragging) {
				for (int i = 0; i < handle_sphere_influenced_idx.size(); i++) {
					VectorD diff = handle_sphere_pos - particles.X(handle_sphere_influenced_idx[i]);
					VectorD change = 10. * diff;
					decay = 3. / (handle_sphere_influenced_radius);
					change *= exp(-decay * handle_sphere_influenced_dist[i]);
					particles.V(handle_sphere_influenced_idx[i]) += weight*change;
				}
			}

			for (int i = 0; i < particles.Size(); i++) {
				if (fixed[i]) {
					particles.V(i) *= 0;
				}
			}
		}
	}

	void Relocate_Handle(void) {
		handle_sphere_pos = init_handle_sphere_pos;
	}

	virtual void Advance(const double dt)
	{
		////Clear forces on particles
		for(int i=0;i<particles.Size();i++){
			particles.F(i)=VectorD::Zero();}

		////Accumulate body forces
		for(int i=0;i<particles.Size();i++){
			particles.F(i)+=particles.M(i)*g;}
		
		//Shape_Match_Basic(dt);
		//Shape_Match_Linear(dt);
		Shape_Match_Quadratic(dt);
		if(useCluster){
			Clusters_Shape_Match_Quadratic(dt);
		}
		//Shape_Match_Quadratic_Plasticity(dt);

		for(int i=0;i<particles.Size();i++){
			particles.V(i)+=particles.F(i)/particles.M(i)*dt;
			particles.V(i)*=exp(-decay*dt);
		}
		
		Test_Specific_Vel_Operations();

		for (int i = 0; i < particles.Size(); i++) {
			particles.X(i) += particles.V(i) * dt;
		}
		
		Particle_Environment_Collision();
	}

	virtual void Particle_Environment_Collision()
	{
		for (int i = 0; i < particles.Size(); i++) {
			double smallest_phi = std::numeric_limits<double>::max();
			VectorD smallest_normal = VectorD::Zero();
			VectorD curr_X = particles.X(i);
			for (int j = 0; j < env_objects.size(); j++) {
				double phi = env_objects[j]->Phi(curr_X);
				if (phi < smallest_phi) {
					smallest_phi = phi;
					smallest_normal = env_objects[j]->Normal(curr_X);
				}
			}
			phis[i] = smallest_phi;
			if (smallest_phi < 0.) {
				VectorD normal_velocity = particles.V(i).dot(smallest_normal) * smallest_normal;
				VectorD tangential_velocity = particles.V(i) - normal_velocity;
				VectorD normal_displacement = smallest_normal * -smallest_phi; // displace this much to be on the boundary
				particles.V(i) = tangential_velocity;
				if (-normal_velocity.dot(smallest_normal) > 0.) {
					particles.V(i) += 0.2 * -normal_velocity;
				}
				particles.X(i) += normal_displacement;
			}
		}
	}

	int Find_Nearest_Nb(VectorD pos) {
		double smallest = std::numeric_limits<double>::max();
		int smallest_nb = -1;
		for (int i = 0; i < particles.Size(); i++) {
			double dist = (particles.X(i) - pos).norm();
			if (dist < smallest) {
				smallest = dist;
				smallest_nb = i;
			}
		}
		return smallest_nb;
	}

	void Move_Left(void) {
		handle_sphere_pos -= VectorD::Unit(0) * dx;
	}
	void Move_Right(void) {
		handle_sphere_pos += VectorD::Unit(0) * dx;
	}
	void Move_Top(void) {
		handle_sphere_pos += VectorD::Unit(1) * dx;
	}
	void Move_Bottom(void) {
		handle_sphere_pos -= VectorD::Unit(1) * dx;
	}
	void Move_Front(void) {
		handle_sphere_pos += VectorD::Unit(2) * dx;
	}
	void Move_Back(void) {
		handle_sphere_pos -= VectorD::Unit(2) * dx;
	}
	void Toggle_On_Off(void) {
		if (!dragging && !just_released) { //if was previously not dragging 
			//regrasp
			init_handle_sphere_pos = handle_sphere_pos;
			handle_sphere_influenced_idx.clear();
			handle_sphere_influenced_dist.clear();
			for (int i = 0; i < particles.Size(); i++) {
				double dist = (particles.X(i) - handle_sphere_pos).norm();
				if (dist <= handle_sphere_influenced_radius) {
					handle_sphere_influenced_idx.push_back(i);
					handle_sphere_influenced_dist.push_back(dist);
				}
			}
		}
		else { // if was previously dragging
			just_released = true;
		}
		dragging = !dragging;
	}

};

#endif
