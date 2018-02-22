function data = apply_calibration (x,y,z,offset,gain,rotation)

% translate to (0,0,0)
x = x - offset(1);
y = y - offset(2);
z = z - offset(3); 

% rotate to XYZ axes
XYZ = [x, y, z] * rotation; 

% reference radius
data.x = XYZ(:,1)/gain(1); 
data.y = XYZ(:,2)/gain(2); 
data.z = XYZ(:,3)/gain(3); 

end