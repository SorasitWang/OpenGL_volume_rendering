
#version 330 core


struct Plane {
	vec3 point;
	vec3 normal;
};
struct AABB {
	vec3 min;
	vec3 max;
	vec3 center;
	// -x +x -y +y -z +z
	Plane planes[6];
};


struct FogVexel {
	vec3 sigma_a;
	vec3 sigma_s;
	float density;
	AABB box;
};
struct Fog {
	vec3 min;
	vec3 max;
	vec3 num;
	vec3 step;
	FogVexel values[220];
};

uniform vec3 viewPos; 
uniform Fog fogBox;
uniform sampler2D texture1;
out vec4 FragColor;

in vec4 ioEyeSpacePosition;
in vec4 pos;
in vec2 texCoord;
// By experiment
float Epsilon = 1e-5;

bool insideAABB(vec3 vecPoint,AABB box)
{
	 if(vecPoint.x > box.min.x-Epsilon && vecPoint.x < box.max.x+Epsilon &&
		vecPoint.y > box.min.y-Epsilon && vecPoint.y < box.max.y+Epsilon &&
		vecPoint.z > box.min.z-Epsilon && vecPoint.z < box.max.z+Epsilon)
		{
			return true;
		}
	return false;
}

bool planeIntersectLine(Plane plane,vec3 p0,vec3 p1,inout vec3 result)
{
	vec3 u = p1 - p0;
    float dot = dot(plane.normal, u);
	if (dot == 0) dot = Epsilon;

    if (abs(dot) > Epsilon){
        // The factor of the point between p0 -> p1 (0 - 1)
        // if 'fac' is between (0 - 1) the point intersects with the segment.
        // Otherwise:
        //  < 0.0: behind p0.
        //  > 1.0: infront of p1.
        vec3 w = p0 - plane.point;
        float fac = -dot(plane.normal, w) / dot;
		if (fac>=-Epsilon && fac <=1+Epsilon){
			u = u * fac;
			//result = vec3(1);
			result =p0 +  u;
			return true;
		}
	}
    // The segment is parallel to plane.
    return false;
}
float lineAABBIntersectDistance(AABB box,vec3 v0,vec3 v1,inout bool isIntersect)
{
	// extract box to 6 planes, find 2(+) planes that intersect with the line then find distance between them
	// if both start and end of line are inside, cal distance between them.

	bool v0Inside = insideAABB(v0,box);
	bool v1Inside =insideAABB(v1,box) ;
	if (v0Inside && v1Inside)
		return distance(v0,v1);

	vec3 whichInside;
	int intersectCount = 1;
	if (v0Inside){
		whichInside = v0;
	}
	else if (v1Inside)
		whichInside = v1;
	else
		intersectCount = 2;

	// make 6 planes from AABB
	vec3 range = (box.max-box.min)/2;
	int c=0;
	for (int i=-1;i<2;i+=2){
		box.planes[c].point = box.center + vec3(i*range.x,0,0);
		box.planes[c++].normal = vec3(i,0,0);
	}
	for (int i=-1;i<2;i+=2){
		box.planes[c].point = box.center + vec3(0,i*range.y,0);	
		box.planes[c++].normal = vec3(0,i,0);
	}
	for (int i=-1;i<2;i+=2){
		box.planes[c].point =box.center + vec3(0,0,i*range.z);
		box.planes[c++].normal = vec3(0,0,i);
	}

	vec3 results[5];
	int reCount = 0;
	vec3 tmp ;
	for (int i=0;i<6;i++){
		
		if (planeIntersectLine(box.planes[i],v0,v1,tmp)){
			if (insideAABB(tmp,box))
				results[reCount++] = tmp;
				}
	}
	// Case of intersect 3 planes?

	// Either intersect 2 points or not intersects.
	if (reCount < intersectCount){
		isIntersect = false;
		return 0;
	}
	
	isIntersect = true;
	if (reCount == 2){
		if (intersectCount == 1)
			return distance(whichInside,results[0]);
		return distance(results[0],results[1]);
	}
	else {
		// Handle only case intersect 3 planes is enough (line in border)
		// 2 of 3 intersect point may be too close, affect the distance
		return max(max(distance(results[0],results[1]),distance(results[0],results[2])),distance(results[1],results[2]));
	}
}

vec3 getFogFactor(FogVexel params, float fogCoordinate)
{
	
	vec3 result = vec3(exp(-fogCoordinate*params.sigma_a.r),
						exp(-fogCoordinate*params.sigma_a.g),
						exp(-fogCoordinate*params.sigma_a.b));

	
	result = vec3(1) - clamp(result, 0.0, 1.0);
	return result;
}

void main()
{
	
    FragColor = vec4(texture(texture1, texCoord).rgb,1);
	
	float stepX2 = fogBox.step.x/2;
	float stepY2 = fogBox.step.y/2;
	float stepZ2 = fogBox.step.z/2;
	float distance=0;
	bool isIntersect = true ;
	for (int i=0;i<fogBox.num.x;i++){
		float offsetX = i*fogBox.step.x;
		for (int j=0;j<fogBox.num.y;j++){
			float offsetY = j*fogBox.step.y;
			for (int k=0;k<fogBox.num.z;k++)
			{
				float offsetZ = k*fogBox.step.z;
				FogVexel thisFog;
				thisFog = fogBox.values[int(i+(fogBox.num.x*j)+(fogBox.num.x*fogBox.num.y)*k)];
				vec3 center = fogBox.min + vec3(stepX2+offsetX,stepY2+offsetY,stepZ2+offsetZ);
				
				AABB box ;
				box.min = vec3(center.x-stepX2,center.y-stepY2,center.z-stepZ2);
				box.max = vec3(center.x+stepX2,center.y+stepY2,center.z+stepZ2);
				box.center = center;
				
				distance = lineAABBIntersectDistance(box, viewPos, pos.xyz,isIntersect);
				if (isIntersect){
					vec3 factor = thisFog.density*getFogFactor(thisFog, distance);
					
					FragColor.r = mix(FragColor.r, thisFog.sigma_s.r, factor.r);
					FragColor.g = mix(FragColor.g, thisFog.sigma_s.g, factor.g);
					FragColor.b = mix(FragColor.b, thisFog.sigma_s.b, factor.b);

				}

			}
		}
	}


	
	
	

    

}