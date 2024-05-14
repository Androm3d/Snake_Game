#version 330 core

in vec3 fvertex;
in vec3 fnormal;

in vec3 fmatamb;
in vec3 fmatdiff;
in vec3 fmatspec;
in float fmatshin;

uniform vec3 colorFocus;
uniform vec3 llumAmbient;
uniform vec3 colorFocus2;
uniform vec3 posFocus;
uniform vec3 posFocus2;

uniform mat4 TG;
uniform mat4 Proj;
uniform mat4 View;

out vec4 FragColor;

vec3 Ambient() {
    return llumAmbient * fmatamb;
}

vec3 Difus(vec3 nSCO, vec3 L, vec3 colFocus) {
    vec3 colRes = vec3(0);
    if (dot (L, nSCO) > 0)
        colRes = colFocus * fmatdiff * dot(L, nSCO);
    return colRes;
}

vec3 Especular(vec3 nSCO, vec3 L, vec4 vertSCO, vec3 colFocus) {
    vec3 colRes = vec3(0);
    if ((dot (nSCO, L) < 0) || (fmatshin == 0))
        return colRes;
    vec3 R = reflect(-L, nSCO);
    vec3 V = normalize(-vertSCO.xyz);
    if (dot (R, V) < 0)
        return colRes;
    float shine = pow(max(0.0, dot(R, V)), fmatshin);
    return(fmatspec * colFocus * shine);
}

void main() {
	//fcolor = color;
    mat3 NormalMatrix = inverse(transpose( mat3(View * TG)));
    vec3 nSCO = NormalMatrix * fnormal;
    nSCO = normalize(nSCO);
    vec4 vertSCO = View * TG * vec4(fvertex, 1.0);
    vec4 posFSCO = View *vec4(posFocus, 1.0);
    vec4 posFSCO2 = View *vec4(posFocus2, 1.0);

    vec3 L = normalize(posFSCO.xyz - vertSCO.xyz);
    vec3 L2 = normalize(posFSCO2.xyz - vertSCO.xyz);
    vec3 fcolor = Ambient() + Difus(nSCO, L, colorFocus) + Difus(nSCO, L2, colorFocus2) + Especular(nSCO, L, vertSCO, colorFocus) + Especular(nSCO, L2, vertSCO, colorFocus2);
	FragColor = vec4(fcolor, 1);
}

