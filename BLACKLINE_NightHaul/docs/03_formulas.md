# 03 — Fórmulas y valores por defecto (BLACKLINE: Night Haul)

Todos los valores son defaults `EditAnywhere` (ver tabla de tuning, sección 9). Unidades: km/h para velocidad de snapshot, m/s y rad/s en física, cm para offsets, kg para masa. Tipos según `Core/BlacklineTypes.h`.

---

## 1. Firma sin blindar (cargo) — `UCargoSimulationComponent::GetUnshieldedSignature01`

**Expresión:** `U = 1 − Π_i ( 1 − sig_i · (1 − sh_i) )`, para i con `bRequiresShielding = true`.

| Símbolo | Tipo | Rango | Descripción |
|---|---|---|---|
| sig_i | float | 0–1 | `ContrabandSignature01` del ítem i |
| sh_i | float | 0–1 | Blindaje aplicado al ítem (0 = ninguno). Default 0 |
| U | float | 0–1 | Firma sin blindar total |

**Rango:** 0 (sin ítems que requieran blindaje o todos blindados) a 1 (algún ítem con sig·(1−sh)=1). Acotado por construcción (producto de factores en [0,1]); sin división.
**Ejemplo:** sig = 0.5 y 0.3, sh = 0 → U = 1 − 0.5·0.7 = **0.65**.

**Firma declarada** (`UManifestDataAsset::ComputeDeclaredSignature01`): misma fórmula sobre las líneas del manifiesto, con sh = 0 y un solo factor por ítem distinto (la cantidad no multiplica).

## 2. Discrepancia de masa manifiesto vs real

**Expresión:** `D = |M_real − M_decl| / max(M_decl, M_MIN)`; `D_eff = max(0, D − D_TOL)`.

| Símbolo | Tipo | Rango | Descripción |
|---|---|---|---|
| M_real | float | ≥0 kg | `Cargo->GetTotalCargoMassKg()` |
| M_decl | float | ≥0 kg | `Manifest->ComputeDeclaredMassKg()` (Σ MassKg·Quantity) |
| M_MIN | float | 200 | Piso del denominador (evita /0 con manifiesto vacío) |
| D_TOL | float | 0.05 | Tolerancia de báscula (5 %) |
| D_eff | float | 0–∞ | Discrepancia efectiva (se satura en F3) |

**Rango:** D ∈ [0, ∞) (sobrecarga ilimitada); se acota con `D_SAT` en F3. Cuenta tanto carga de más como de menos (manifiesto inflado también es sospechoso).
**Ejemplo:** M_real 1500, M_decl 1200 → D = 0.25, D_eff = **0.20**.

## 3. Sospecha del retén — `ACheckpointController::GetSuspicion01`

**Expresión:**
```
T      = 1 − K_FORGE · Q                                  (confianza documental)
t_spd  = W_SPD · clamp((v − V_SAFE)/(V_MAX − V_SAFE), 0, 1)
t_lgt  = W_LGT · (bHeadlightsOn ? 0 : 1)
t_dmg  = W_DMG · DamageRatio01
t_int  = W_INT · (1 − CargoIntegrity01)
sigEx  = max(0, U·(1 − JAM_ATT·bSignalJammerOn) − Sig_decl)
t_sig  = W_SIG · clamp(sigEx, 0, 1) · T
t_dsc  = W_DSC · clamp(D_eff / D_SAT, 0, 1) · T
t_frg  = W_FRG · (1 − Q)
S      = clamp(t_spd + t_lgt + t_dmg + t_int + t_sig + t_dsc + t_frg, 0, 1)
```

| Símbolo | Tipo | Rango | Descripción |
|---|---|---|---|
| v | float | 0–∞ km/h | `SpeedBeforeStopKmh` |
| Q | float | 0–1 | `ForgeryQuality01` del manifiesto |
| U, Sig_decl | float | 0–1 | F1 real / declarada |
| D_eff | float | ≥0 | F2 |
| DamageRatio01, CargoIntegrity01 | float | 0–1 | del `FSuspicionSnapshot` |
| bHeadlightsOn, bSignalJammerOn | bool | — | del snapshot |
| W_SPD / W_LGT / W_DMG / W_INT / W_SIG / W_DSC / W_FRG | float | 0.10 / 0.08 / 0.10 / 0.04 / 0.28 / 0.30 / 0.10 | Pesos (suman 1.00) |
| V_SAFE / V_MAX | float | 15 / 60 km/h | Rango de la rampa de velocidad |
| K_FORGE | float | 0.70 | Cuánto reduce la falsificación perfecta los términos físico-documentales |
| JAM_ATT | float | 0.60 | Atenuación del inhibidor sobre U |
| D_SAT | float | 0.40 | D_eff que satura el término de masa |
| S | float | 0–1 | Sospecha |

**Rango:** mínimo 0 (v ≤ 15, luces on, sin daño, integridad 1, U ≤ Sig_decl, D ≤ 5 %, Q = 1). Máximo 1.00 (Q = 0, v ≥ 60, luces off, daño 1, integridad 0, sigEx = 1, D_eff ≥ 0.40: los pesos suman exactamente 1). Sin /0 (T ∈ [0.30, 1]). Nota de diseño: luces apagadas bajan la persecución (F7) pero suben sospecha aquí; tensión intencional.
**Ejemplo:** v=40, luces on, daño 0.2, integridad 0.9, U=0.5, jammer on, Sig_decl=0.1, Q=0.6, M_real 1500/M_decl 1200.
T = 0.58; t_spd = 0.0556; t_lgt = 0; t_dmg = 0.020; t_int = 0.004; sigEx = 0.5·0.4 − 0.1 = 0.1 → t_sig = 0.0162; D_eff/D_SAT = 0.5 → t_dsc = 0.087; t_frg = 0.040. **S = 0.223 → CleanPass.**

## 4. Umbrales de veredicto y nivel de persecución

**Expresión:**
```
Verdict = CleanPass            si S <  TH_SECONDARY
          SecondaryInspection  si TH_SECONDARY ≤ S < TH_PURSUIT
          PursuitAlert         si S ≥ TH_PURSUIT
S2 = clamp( S − t_spd + (K_SEC − 1)·(t_sig + t_dsc), 0, 1 )   (reinspección: parado y escaneo más profundo)
  tras la inspección secundaria: S2 ≥ TH_PURSUIT → PursuitAlert, si no → CleanPass
L  = clamp( 1 + floor( (S − TH_PURSUIT)/(1 − TH_PURSUIT) · 3 ), 1, 3 )   (argumento de StartPursuit)
```

| Símbolo | Tipo | Rango | Descripción |
|---|---|---|---|
| TH_SECONDARY | float | 0.35 | Umbral a inspección secundaria |
| TH_PURSUIT | float | 0.65 | Umbral a alerta de persecución |
| K_SEC | float | 1.30 | Multiplicador de escaneo profundo |
| JITTER | float | ±0.03 | Ruido uniforme opcional sobre S (0 = determinista) |
| L | int | 1–3 | Nivel de persecución |

**Rango:** L = 1 para S ∈ [0.65, 0.767), 2 para [0.767, 0.883), 3 para ≥ 0.883 (S = 1 daría 4, se recorta a 3). Requisito: TH_SECONDARY < TH_PURSUIT.
**Ejemplo:** S = 0.50 → Secondary; con t_spd = 0.05, t_sig + t_dsc = 0.30 → S2 = 0.50 − 0.05 + 0.09 = 0.54 → CleanPass. S = 0.80 → PursuitAlert, L = 1 + floor(0.15/0.35·3) = **2**.

## 5. Desplazamiento del centro de masa por carga

**Expresión:** `CoM = ( M_VAN·C_VAN + Σ m_i·(p_i + s_i) ) / ( M_VAN + Σ m_i )`, luego clamp por eje.

| Símbolo | Tipo | Rango | Descripción |
|---|---|---|---|
| M_VAN | float | 2400 kg | Masa de la furgoneta vacía |
| C_VAN | FVector | (0,0,60) cm | CoM base en espacio local (X adelante) |
| m_i | float | >0 kg | `MassKg` del ítem |
| p_i | FVector | cm | `LocalOffsetCm` del slot |
| s_i | FVector | |s_y| ≤ SHIFT_MAX | Desplazamiento por deslizamiento de carga (F6) |
| CoM | FVector | ver clamp | Entrada de `SetCenterOfMass` |
| COM_LIM | FVector | X ±120, Y ±60, Z 20…140 cm | Clamp de seguridad para estabilidad Chaos |

**Rango:** sin carga = C_VAN. Siempre dentro de COM_LIM (clamp); denominador ≥ M_VAN > 0. Se recalcula solo en AddCargo/ClearCargo/shift, no por tick.
**Ejemplo:** 500 kg en (−100, 40, 30): X = −50000/2900 = −17.2; Y = 20000/2900 = 6.9; Z = (144000+15000)/2900 = 54.8 → **CoM = (−17.2, 6.9, 54.8) cm**.

## 6. Fuerza G lateral, deslizamiento y daño por fragilidad

**Expresión:**
```
a_lat  = |v_fwd · ω_yaw|            (m/s²)
G_raw  = a_lat / 9.81
G      = G_prev + α·(G_raw − G_prev)              (suavizado EMA a 20 Hz)
G_th_i = G_BASE · (1 − K_FRAG_TH · F_i)           (umbral por ítem)
dI_i/dt = − K_G · F_i · max(0, G − G_th_i)        (daño continuo, por segundo)
ΔI_i   = − F_i · K_IMP · clamp((v_n − V_IMP_MIN)/V_IMP_REF, 0, 2)²     (ApplyImpact)
I_i    = clamp(I_i + Δ, 0, 1)
I      = Σ(Value_i · I_i) / Σ Value_i     (GetCargoIntegrity01; si ΣValue = 0, ponderar por masa)
Shift: si G ≥ G_SHIFT y cooldown listo → Severity = clamp((G − G_SHIFT)/(G_SHIFT_MAX − G_SHIFT), 0, 1);
       s_y += sign(a_lat) · SHIFT_DIST · Severity (|s_y| ≤ SHIFT_MAX); lanza OnCargoShifted(Severity)
```

| Símbolo | Tipo | Rango | Descripción |
|---|---|---|---|
| v_fwd, ω_yaw | float | m/s, rad/s | Velocidad longitudinal y velocidad de giro del van |
| α | float | 0.35 | Factor EMA (≈ 0.1 s de respuesta a 20 Hz) |
| F_i | float | 0–1 | `Fragility01` |
| G_BASE | float | 0.80 | G de umbral con fragilidad 0 |
| K_FRAG_TH | float | 0.60 | Cuánto baja el umbral la fragilidad máxima (F=1 → 0.32 G) |
| K_G | float | 0.20 | Integridad perdida por s por G sobre el umbral por unidad de F |
| v_n | float | ≥0 m/s | `ImpactSpeedMs` (componente normal) |
| V_IMP_MIN / V_IMP_REF | float | 2 / 10 m/s | Zona muerta y escala de impacto |
| K_IMP | float | 0.50 | Daño de impacto a 12 m/s con F = 1 es 0.5 |
| G_SHIFT / G_SHIFT_MAX | float | 0.75 / 1.25 | Rango de deslizamiento |
| SHIFT_DIST / SHIFT_MAX | float | 25 / 40 cm | Paso y tope de deslizamiento |
| SHIFT_COOLDOWN | float | 2.0 s | Entre eventos de shift |
| I, I_i | float | 0–1 | Integridad total / por ítem |

**Rango:** G ∈ [0, ∞); daño ≥ 0 siempre, I recortada a [0,1]. Impacto: el factor clamp² ∈ [0,4] → ΔI hasta −2·F (satura en 0). Daño de impacto mortal desde ≈ 12 m/s con F alta.
**Ejemplo:** v=15 m/s, ω=0.6 rad/s → G = 0.917. Ítem frágil F = 0.8: G_th = 0.416; dI/dt = −0.2·0.8·0.501 = **−0.080/s**. Ítem estándar F = 0.1: G_th = 0.752, −0.0033/s. Shift: Severity = (0.917−0.75)/0.5 = **0.334**. Impacto a 12 m/s, F=0.8: −0.8·0.5·1.0 = **−0.40**.

## 7. Heat de persecución y enfriado — `UPursuitDirectorSubsystem` (timer, sin Tick)

**Expresión (cada ΔT = 0.5 s):**
```
H0      = clamp(H_BASE + H_PER_LEVEL · L, 0, 1)             (StartPursuit)
C       = 1 + K_ENG·(motor apagado) + K_LGT·(luces apagadas)  (mult. de enfriado)
H'      = clamp( H + ΔT · ( visible ? R_UP : −R_DOWN · C ), 0, 1 )
Transiciones:
 Hunting  → Searching : no visible ≥ T_LOSE_SIGHT
 Searching→ Hunting   : visible (H += REACQ_BONUS)
 Searching→ Lost      : H ≤ H_LOST  o  tiempo en Searching ≥ T_SEARCH_MAX
 Lost     → None      : tras T_LOST_HOLD; H = 0
```

| Símbolo | Tipo | Rango | Descripción |
|---|---|---|---|
| H_BASE / H_PER_LEVEL | float | 0.35 / 0.15 | Heat inicial (L1 = 0.50, L3 = 0.80) |
| R_UP | float | 0.06 /s | Subida con el objetivo a la vista |
| R_DOWN | float | 0.025 /s | Enfriado base fuera de vista |
| K_ENG / K_LGT | float | 1.5 / 1.0 | Aporte de motor apagado / luces apagadas |
| T_LOSE_SIGHT | float | 4 s | Sin visión para pasar a Searching |
| REACQ_BONUS | float | 0.10 | Heat al reencontrar |
| H_LOST | float | 0.15 | Heat para dar por perdido |
| T_SEARCH_MAX | float | 45 s | Tope de búsqueda |
| T_LOST_HOLD | float | 3 s | Estado Lost antes de None |
| H | float | 0–1 | `GetHeat01()` |

**Rango:** H acotado a [0,1]. C ∈ [1, 3.5], así el enfriado va de 0.025 a 0.0875 /s. Sin heat ≥ 1 fijo: a 1.0 la IA puede escalar (decisión de IA, no de esta fórmula).
**Ejemplo:** L = 2 → H0 = 0.65. A la vista 10 s: +0.60 → 1.0 (clamp). Escondido con motor y luces apagados desde H = 0.8: 0.0875/s → llega a 0.15 en **7.4 s**; con todo encendido, 0.025/s → 26 s.

## 8. Fricción por superficie

**Expresión:**
```
μ_s     = min over volúmenes activos de FrictionMultiplier     (la más restrictiva manda, no se multiplica)
K_DRIVE = bRequires4WD && Modo == RWD ? K_RWD_PENALTY : 1
K_DIFF  = DiffLock && Surface ∈ {Gravel, Mud, Forest} ? K_DIFF_BONUS : 1
F_eff   = clamp( μ_s · K_DRIVE · K_DIFF, F_MIN, F_MAX )
Wheel.FrictionForceMultiplier = WHEEL_FRIC_BASE · F_eff
Atascado: sink += dt · SinkDepth/T_STUCK  si bRequires4WD ∧ RWD ∧ throttle > 0.5 ∧ v < 5 km/h; sink −= dt·SINK_RECOVER al moverse; atascado si sink ≥ SinkDepth.
```

| Símbolo | Tipo | Rango | Descripción |
|---|---|---|---|
| μ_s | float | 0.15–1.2 | FrictionMultiplier de la superficie (defaults abajo) |
| K_RWD_PENALTY | float | 0.50 | Penalización por tracción trasera en superficie 4WD |
| K_DIFF_BONUS | float | 1.15 | Bono de bloqueo de diferencial |
| F_MIN / F_MAX | float | 0.15 / 1.20 | Límites del resultado |
| WHEEL_FRIC_BASE | float | 3.0 | Base de las ruedas (UBlacklineWheel*) |
| SinkDepth | float | cm | `StuckSinkDepthCm` del volumen |
| T_STUCK | float | 3 s | Tiempo de hundimiento completo |

**Defaults por superficie:**

| Superficie | FrictionMultiplier | bRequires4WD | StuckSinkDepthCm |
|---|---|---|---|
| Asphalt | 1.00 | no | 0 |
| WetAsphalt | 0.65 | no | 0 |
| Gravel | 0.70 | no | 0 |
| Mud | 0.40 | sí | 25 |
| Forest | 0.55 | sí | 10 |

**Rango:** F_eff ∈ [0.15, 1.2]. Sin volúmenes activos: μ_s = 1.
**Ejemplo:** Barro, RWD: 0.40·0.5 = **0.20**. Barro, 4WD: 0.40. Barro, 4WD + diff lock: 0.40·1.15 = **0.46**. Atascado en barro con RWD: sink 25 cm en 3 s.

---

## 9. Tabla de tuning

| Parámetro | Default | Rango seguro | Efecto |
|---|---|---|---|
| W_SPD/LGT/DMG/INT/SIG/DSC/FRG | .10/.08/.10/.04/.28/.30/.10 | suma = 1.0 (si ≠ 1 recalcular max S) | Qué delata más al jugador |
| V_SAFE / V_MAX | 15 / 60 | 5–30 / 40–100 | Tolerancia a llegar rápido al retén |
| K_FORGE | 0.70 | 0.4–0.9 | Valor de la falsificación |
| JAM_ATT | 0.60 | 0.3–0.9 | Fuerza del inhibidor |
| D_TOL / D_SAT / M_MIN | 0.05 / 0.40 / 200 | 0–0.15 / 0.2–0.8 / 100–500 | Sensibilidad a la masa |
| TH_SECONDARY / TH_PURSUIT | 0.35 / 0.65 | 0.2–0.5 / 0.5–0.85 (mín. +0.15 entre ellos) | Dificultad general del retén |
| K_SEC / JITTER | 1.30 / 0.03 | 1.0–1.6 / 0–0.08 | Castigo de reinspección / aleatoriedad |
| M_VAN / C_VAN | 2400 / (0,0,60) | 1500–4000 / Z 40–90 | Base de la estabilidad |
| COM_LIM | ±120/±60/20…140 | — | Estabilidad numérica Chaos |
| α, G_BASE, K_FRAG_TH | 0.35, 0.80, 0.60 | 0.2–0.6, 0.6–1.1, 0.3–0.8 | Cuándo la carga empieza a sufrir |
| K_G, K_IMP | 0.20, 0.50 | 0.05–0.5, 0.2–1.0 | Dureza del daño |
| G_SHIFT, SHIFT_DIST, SHIFT_MAX | 0.75, 25, 40 | 0.5–1.0, 10–40, 20–60 | Frecuencia del deslizamiento de carga |
| H_BASE, H_PER_LEVEL | 0.35, 0.15 | 0.2–0.5, 0.05–0.2 | Intensidad inicial de la persecución |
| R_UP, R_DOWN | 0.06, 0.025 | 0.03–0.12, 0.01–0.05 | Ritmo de la persecución |
| K_ENG, K_LGT | 1.5, 1.0 | 0–3 | Valor del sigilo (motor/luces apagados) |
| T_LOSE_SIGHT, T_SEARCH_MAX, T_LOST_HOLD | 4, 45, 3 s | 2–8, 20–90, 1–6 | Duración de fases |
| μ por superficie | ver F8 | 0.2–1.2 | Agarre por terreno |
| K_RWD_PENALTY, K_DIFF_BONUS | 0.50, 1.15 | 0.3–0.8, 1.0–1.4 | Importancia del 4WD/diferencial |
| T_STUCK | 3 s | 1–8 | Rapidez de atascarse |

## 10. Dudas abiertas
1. El contrato no incluye masa real en `FSuspicionSnapshot` ni un mecanismo de blindaje (`sh_i`): `ACheckpointController` debe leer `Cargo->GetTotalCargoMassKg()` directo y `sh_i` queda en 0 hasta que diseño defina el blindaje.
2. `DamageRatio01` del van: origen sin definir (se sugiere acumular daño de choques ÷ umbral).
