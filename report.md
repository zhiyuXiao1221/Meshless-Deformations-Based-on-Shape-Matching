

# **Dartmouth CS189 Final Project** Report

Implementation of Müller, Matthias, et al. "Meshless deformations based on shape matching." ACM transactions on graphics (TOG) 24.3 (2005): 471-478.

**Paper link:**

https://www.cs.drexel.edu/~david/Classes/Papers/MeshlessDeformations_SIG05.pdf

**Project Git Repository:**

https://github.com/zhiyuXiao1221/Meshless-Deformations-Based-on-Shape-Matching

**Presentation Slide:**

https://docs.google.com/presentation/d/132QbHPjNALq0Bzg6jysMY3O5C7fggy7zpvF8CZLaKBc/edit#slide=id.p

### Simulation overview 

We used the method described in this paper to simulate several deformable objects. The primary concept is to replace energy with geometric limitations and forces with distances between current and goal positions. Basic deformation, linear deformation, and quadratic deformation are the three types of deformations we used. In our implementation, we also took into account the plasticity property. We also tried cluster deformation to get a better outcome. To ensure that our method is correct, we put it to the test on three different scenarios: a basic cube, a lattice cube, and a bunny mesh. The end findings reveal that this method is capable of simulating the effect of physical-based deformation methods, and the deformation reaction time is also promising.

### Implementation details

#### Basic Algorithm and Math Models  

The basic idea behind this geometric algorithm is simple. All we need as input is a set of particles with masses $m_i$and an initial positions $x_i^0$. The particles are simulated as a simple particle system without particle-particle interactions, but including response to collisions with the environment and including external forces such as gravity. After each time step, each particle is pulled towards its goal position $g_i$. 

<img src="/Users/dhyscuduke/Library/Application Support/typora-user-images/image-20220312202812484.png" alt="image-20220312202812484" style="zoom:50%;" />

The most important goal is to keep the original contour. As a result, form matching is limited to only two translations and one rotation. By translating the original item to the zero-point, rotating it, and then translating it to the target point, the limits may be simply demonstrated. We try to find a rotation matrix **R** and two translation vectors $t$ and $t_0$0 which minimise the following

equation:

<img src="/Users/dhyscuduke/Library/Application Support/typora-user-images/image-20220312204402280.png" alt="image-20220312204402280" style="zoom:40%;" />

<img src="/Users/dhyscuduke/Library/Application Support/typora-user-images/image-20220312204508858.png" alt="image-20220312204508858" style="zoom:50%;" />

Then we relax the problem of finding the optimal rota tion matrix **R** to finding the optimal linear transformation **A**. Now,

the term to be minimized is:

<img src="/Users/dhyscuduke/Library/Application Support/typora-user-images/image-20220312204636171.png" alt="image-20220312204636171" style="zoom:60%;" />

The optimal **A** is:

<img src="/Users/dhyscuduke/Library/Application Support/typora-user-images/image-20220312204808557.png" alt="image-20220312204808557" style="zoom:50%;" />

The goal position can be calculated as:

<img src="/Users/dhyscuduke/Library/Application Support/typora-user-images/image-20220312204920755.png" alt="image-20220312204920755" style="zoom:50%;" />

<img src="/Users/dhyscuduke/Library/Application Support/typora-user-images/image-20220312204905093.png" alt="image-20220312204905093" style="zoom:50%;" />

<img src="/Users/dhyscuduke/Library/Application Support/typora-user-images/image-20220312204850208.png" alt="image-20220312204850208" style="zoom:50%;" />

#### Linear, Quatratic Deformation

**Linear Deformation**

The parameter β is used to adjust the degree of influence the linear transformation has on shape matching, and so the equation for calculating the goal position for a particle becomes:

<img src="/Users/dhyscuduke/Library/Application Support/typora-user-images/image-20220312205057868.png" alt="image-20220312205057868" style="zoom:50%;" />

**Quatratic Deformation**

Linear transformations can only represent shear and stretch. To extend the range of motion by twist and bending modes, we move from linear to quadratic transformations.

<img src="/Users/dhyscuduke/Library/Application Support/typora-user-images/image-20220312205727203.png" alt="image-20220312205727203" style="zoom:50%;" />

<img src="/Users/dhyscuduke/Library/Application Support/typora-user-images/image-20220312205454886.png" alt="image-20220312205454886" style="zoom:50%;" />

<img src="/Users/dhyscuduke/Library/Application Support/typora-user-images/image-20220312205753253.png" alt="image-20220312205753253" style="zoom:40%;" />

Finally, we use $\beta \tilde{A} +(1-\beta) \tilde{R}$ to compute the goal shape.



#### Clusters Deformation

We partition the space around a particular surface mesh into overlapping cubical regions on a regular basis. We create a cluster for each zone that contains all of the vertices in that region. The original shape of each cluster is matched with its actual shape at each integration step:

<img src="/Users/dhyscuduke/Library/Application Support/typora-user-images/image-20220312205916483.png" alt="image-20220312205916483" style="zoom:40%;" />

#### Code Implementation

We mainly wrote two files, one is ParticleDeformable.h and ParticleDeformableDriver.h. 

In ParticleDeformable.h, we implemented this algorithm and the time integration part.

In Initialize() function, we will calculate the $A_{qq}$ and $t_0$ in advance, and in each timestep, we will update the $t_i$ , $A_{pq}$, $S$ And $R$ to update the goal positions $g_i$ .

In ParticleDeformableDriver.h, we utilized the obj loader to help us import our mesh data in the Initialize() function, and then we synchronized simulation and visualization data in the Initialize Data() and Sync Simulation And Visualization Data() functions. The simulation timesteps are done in the Advance() method. In addition, we register a number of keyboard command routines to round out the keyboard interactive dragging demonstration. We also utilize several OpenGLShaderLibrary functions to initialize shaders and attach them to our model in order to render our rabbit mesh.

### Solved challenges 

How to properly choose a subset of vertices that needed to be treated as guidance particles.

How to properly select a subset vertices need to be animated as particles

Tuned different parameters to achieve a good simulation result

### Contributions 

Yitong

- Linear Deformation
- Quatratic Deformation
- Plasticity
- Lattice Interactive Deformation
- Bug fixing

Zhiyu：

- Basic Defomation

- Clusters Deformation

- Mesh Obj Loader

- Project deployment

  

  

  



