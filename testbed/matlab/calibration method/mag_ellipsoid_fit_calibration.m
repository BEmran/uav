function [offset,gain,rotation] = mag_ellipsoid_fit_calibration(data)
% reference: http://www.st.com/content/ccc/resource/technical/document/design_tip/group0/a2/98/f5/d4/9c/48/4a/d1/DM00286302/files/DM00286302.pdf/jcr:content/translations/en.DM00286302.pdf

% Prepare the data
D = [data.mx data.my data.mz];

% Ellipsoid fit method
%[ofs,gain,rotM] = ellipsoid_fit(D,2);
[offset,gain,rotation] = ellipsoid_fit2(D);

% Rescale
reference = (sum(gain)/3); % rescale the gains 
gain = gain / reference;

end