# NavMeshGenerator
Made by Stegen Sander

## Introduction
The video below is a small demonstration of the finished project.
* The nav mesh support:
  *	Triangulation by ear clipping.
  *	Insetting of vertices to avoid wall collisions.
  *	Meshes with holes/obstacles inside </br>
  
* The nav mesh does not support:
  * Overlapping obstacles
  * Obstacle outside of the boundaries
  </br>
![](GitHubResources/GithubIntro.gif)

## The ear clipping algorithm
The ear clipping algorithm is an algorithm used for triangulation of polygon which is what we want, but there are a few complications with using this.</br></br>
How does the algorithm work?</br>
The algorithm takes a list of points/vertices, then it loops over all points and checks if 3 adjacent point form an ear. An ear is defined as a convex triangle that can be formed without being obstructed by any other point in the mesh. Once we have an ear, we know that the vertices of this ear can be triangulated, we store the connection, remove the ear from the mesh and continue.
After clipping all the ears from our mesh we end up with a triangulated polygon</br></br>
![](GitHubResources/EarclippingExample.png)

### Meshes with a hole
Although ear clipping works fine on regular meshes, it does not work properly on meshes with a hole because the holes are not part of the original list of vertices. To use the ear clipping on meshes with a hole, we first need to get rid of the holes.</br></br>
We can accomplish this by finding ourselves a point to attach our hole to. Let us take the mesh (rectangle) and the hole (triangle) pictured below. to connect the hole to the mesh it looks quite easy for the human eye but is quite difficult for computers. One of the simpler to connect the hole is finding the most right point of our hole and from this point we draw a line to the right. At some point the line will intersect with the outside mesh. When we found this intersection point, we can trace along this line until we find a point that it part of the outside mesh. We now want to insert the vertices of the hole after the correct point in our vertices list. When doing this correctly we should end up with a polygon without a hole like shown below</br></br>
![](GitHubResources/EarclippingWithHoles.png)

### Obstructed connection
The method of connecting a whole to the outside mesh is also not waterproof. When working with multiple holes the connection we want to form with the outside mesh can be obstructed like shown in the image below. </br></br>
To prevent this form happening we want to check if we can see the vertex of the outside mesh we want to connect to and if this connection is not possible, we actually want to connect to the hole that is obstructing us.</br></br>
![](GitHubResources/ObstructedConnection.png)

### Multiple connection to the same point
There is another issue that (might) needs attention. This problem is depending on the implementation you are using.</br></br>
When connecting 2 holes to the same points be careful in which order they get attached. Look at the examples below and see that there are 2 ways of connection to the same point. 1 of the 2 ways clearly messes with the order your vertex list. To prevent this note that (depending on the winding of your polygon) the lower hole always has to come first in the final vertices list</br></br>
![](GitHubResources/PointWithMultipleConnections.png)





