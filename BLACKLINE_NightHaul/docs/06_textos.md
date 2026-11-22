# 06 — Textos (BLACKLINE: NIGHT HAUL)

Todo texto es ficticio. Claves de string-table: tabla `ST_Blackline`, formato `BL_<AREA>_<ID>`.
Variables con nombre: `{company}`, `{route}`, `{plate}`. Máx. 120 caracteres por línea.
Valores de `ForgeryQuality01` y cantidades son ejemplos para iterar; diseño los define.

## 1. Manifiestos falsificados (ejemplos para `UManifestDataAsset`)

### DA_Manifest_Agro
- ManifestId: `Manifest_Agro01` | Clave empresa: `BL_MAN_AGRO_COMPANY`
- CompanyName: Frutícola Valle Sereno S.A.
- RouteCode: `VS-14-N` (clave `BL_MAN_AGRO_ROUTE`)
- DeclaredCargo (Standard):
  - 40 x Caja de cerezas de exportación
  - 12 x Pallet de cartón de embalaje
- ForgeryQuality01: 0.8 (ejemplo)
- Notes (`BL_MAN_AGRO_NOTES`): "Carga seca. Sin cadena de frío. Entrega antes del amanecer."

### DA_Manifest_Medico
- ManifestId: `Manifest_Med01` | Clave empresa: `BL_MAN_MED_COMPANY`
- CompanyName: Laboratorios Austral Ltda.
- RouteCode: `LA-07-S` (`BL_MAN_MED_ROUTE`)
- DeclaredCargo (Fragile):
  - 20 x Caja de vidrio para laboratorio
  - 6 x Bandeja de instrumental estéril
- ForgeryQuality01: 0.55 (ejemplo)
- Notes (`BL_MAN_MED_NOTES`): "Frágil. No apilar. Conductor autorizado: titular."

### DA_Manifest_Mineria
- ManifestId: `Manifest_Min01` | Clave empresa: `BL_MAN_MIN_COMPANY`
- CompanyName: Servicios Mineros Cordillera SpA
- RouteCode: `CM-22-E` (`BL_MAN_MIN_ROUTE`)
- DeclaredCargo (Standard):
  - 30 x Repuesto de maquinaria pesada
  - 8 x Bidón de lubricante industrial
- ForgeryQuality01: 0.35 (ejemplo; sello torcido, fecha repetida)
- Notes (`BL_MAN_MIN_NOTES`): "Retorno de faena. Peso verificado en báscula de origen."

### DA_Manifest_Museo
- ManifestId: `Manifest_Mus01` | Clave empresa: `BL_MAN_MUS_COMPANY`
- CompanyName: Fundación Patrimonio del Sur
- RouteCode: `PS-03-C` (`BL_MAN_MUS_ROUTE`)
- DeclaredCargo (Fragile):
  - 4 x Cajón de réplicas para exhibición
  - 2 x Vitrina desmontada
- ForgeryQuality01: 0.65 (ejemplo)
- Notes (`BL_MAN_MUS_NOTES`): "Réplicas sin valor comercial. Traslado nocturno por solicitud del museo."

## 2. Descripciones de las 4 cargas ficticias (`UCargoItemDataAsset::DisplayName` + descripción)

Solo texto narrativo: sin cifras ni efectos mecánicos (los define diseño).

| Clave | Nombre | Clase | Descripción |
|---|---|---|---|
| `BL_CARGO_CRYO_NAME` / `_DESC` | Cápsula Criogénica "Escarcha" | Cryogenic | Tubo sellado de bruma blanca. Alguien pagó mucho para que nadie pregunte qué duerme dentro. |
| `BL_CARGO_RELIC_NAME` / `_DESC` | Relicario de la Hermandad Gris | Relic | Caja de plomo con grabados gastados. Pesa distinto cada vez que la levantas. |
| `BL_CARGO_AI_NAME` / `_DESC` | Servidor Opaco "Mnemosina" | AIServer | Rack sin etiquetas, tibio al tacto. A veces el ventilador cambia de ritmo cuando le hablas. |
| `BL_CARGO_VOL_NAME` / `_DESC` | Barril de Reactivo Inestable | Volatile | Barril abollado con precinto improvisado. Huele a metal quemado y a mala decisión. |

## 3. Guardia del retén (20 líneas)

Orador: Guardia del retén. Contexto: `ACheckpointController`, zona de inspección. Tono: cansado, seco, profesional.

### Saludo (estado Approaching)
| Clave | Línea |
|---|---|
| `BL_GUARD_GREET_01` | "Buenas noches. Detenga el motor y apague las luces." |
| `BL_GUARD_GREET_02` | "Alto ahí. Retén de control, documentos en mano." |
| `BL_GUARD_GREET_03` | "A ver, acérquese despacio. Ventana abajo." |
| `BL_GUARD_GREET_04` | "Turno largo y camino malo. Muéstreme el manifiesto de {company}." |
| `BL_GUARD_GREET_05` | "Buenas. Ruta {route}, ¿correcto? Deme los papeles." |

### Sospecha (estado Inspecting; inspección secundaria)
| Clave | Línea |
|---|---|
| `BL_GUARD_SUSP_01` | "Este sello se ve raro. ¿Seguro que es de {company}?" |
| `BL_GUARD_SUSP_02` | "Venía rápido para ir con carga frágil, ¿no?" |
| `BL_GUARD_SUSP_03` | "Quédese quieto. Voy a revisar la parte de atrás." |
| `BL_GUARD_SUSP_04` | "Algo no cuadra entre el papel y lo que marca el escáner." |
| `BL_GUARD_SUSP_05` | "Apague ese aparato que tiene en el tablero. Ahora." |

### Aprobado (CleanPass)
| Clave | Línea |
|---|---|
| `BL_GUARD_PASS_01` | "Todo en orden. Puede seguir." |
| `BL_GUARD_PASS_02` | "Documentos al día. Buen viaje." |
| `BL_GUARD_PASS_03` | "Listo. Levanto la barrera, no se demore." |
| `BL_GUARD_PASS_04` | "Pase nomás. Y maneje con cuidado, el camino está feo." |
| `BL_GUARD_PASS_05` | "Está limpio. Siguiente." |

### Alerta (PursuitAlert)
| Clave | Línea |
|---|---|
| `BL_GUARD_ALERT_01` | "¡Alto! ¡Bájese del vehículo!" |
| `BL_GUARD_ALERT_02` | "Central, tengo una furgoneta sospechosa, placa {plate}. ¡Va a huir!" |
| `BL_GUARD_ALERT_03` | "¡Esos papeles son falsos! ¡Cierren la ruta!" |
| `BL_GUARD_ALERT_04` | "¡No se mueva! ¡Todas las unidades, persecución!" |
| `BL_GUARD_ALERT_05` | "¡Barrera abajo! ¡Que no salga de aquí!" |

## Notas
- Mapeo de veredicto: GREET = `Approaching`; SUSP = `SecondaryInspection`; PASS = `CleanPass`; ALERT = `PursuitAlert`.
- Pendiente para narrativa/diseño: confirmar lista final de 4 cargas (se eligieron Cryogenic, Relic, AIServer, Volatile) y tono de la ambientación.
