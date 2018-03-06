function data = apply_calibration (x,y,z,offset,rotation)

XYZ = [x(:) y(:) z(:)]'; % 3xn

offset = offset(:);  % 3x1

XYZ = rotation * (XYZ - offset);

% make it in nx3
data.x = XYZ(1,:); 
data.y = XYZ(2,:); 
data.z = XYZ(3,:); 

end