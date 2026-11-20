# Recorrido del juego (3 niveles, circular) — 19-11-2026

```
 HOSPITAL (inicio) ──glitch+TV──▶ PARQUE ──portón del castillo, glitch+TV──▶ CASONA
        ▲                           ▲                                          │
        │                           └──────── ático, glitch+TV ◀───────────────┘
        └──── glitch+TV (final de la cabina "sala") ──── PARQUE
```

| # | Tramo | Cómo se dispara | Estado |
|---|---|---|---|
| 1 | **Hospital → Parque** | Tras el glitch (`SalidaHecha`), cruzar `DA_Tuning.Salida_Y` (-12600) = final del Ala de Internación → `BP_GestorNivel.VigilarSalida` guarda y lanza `WBP_Transicion.IniciarTV` | ✅ (v0.3.77) |
| 2 | **Parque → Casona** | `BP_PuertaCastillo` frente al portón (0,-12140): guarda, `SetDestino "L_Casona"`, `IniciarTV` | ✅ (v0.3.80) |
| 3 | **Casona → Parque** | `BP_PuertaNivel` (ático): flag `VuelveDeCasona`, `SetDestino "L_Parque"`. El parque te deja frente al castillo (0,-12000) | ✅ (v0.3.80) |
| 4 | **Parque → Hospital (final)** | Cabina VIDEO → elegir "sala" (`EleccionSala`): flag `PersecucionFinal`, guarda, `IniciarTV` → `L_Hospital` | ✅ (v0.3.75) |
| 5 | **Hospital, regreso** | `BP_GestorNivel` (flag `PersecucionFinal`): aparece al fondo del Ala (0,-11800) mirando al norte, el payaso a 10 m detrás (0,-12900, 5,2 m/s). Hay que correr **todo el hospital** (≈ 145 m) hasta el Pabellón 4 | ✅ (v0.3.81) |
| 6 | **Expediente y 3 finales** | Sala del Pabellón 4 (-680,2700): leer el expediente → 62 páginas = verdadero / ≥ 31 = "Lo que recuerdas" / < 31 = "Expediente" | ✅ ya existía |

## Flags de la partida (`BP_GI_MundoMagico.Flags`)
`EntroCastillo` · `VuelveDeCasona` · `PersecucionFinal` (los dos últimos cambian dónde apareces al cargar el mapa).

## Decisiones abiertas (CTO)
1. **¿Qué da por terminado el parque?** Hoy el paso 4 depende de la cabina (cintas vistas). Si quieres que sea "juntar las páginas de la Casona", conviene un contador de páginas (16–40) que habilite la cabina.
2. **Casona: ¿se entra una sola vez?** Hoy el portón siempre funciona. Se puede cerrar tras volver (flag `VuelveDeCasona`).
3. **Pabellón 4 como inicio:** el regreso termina en la Sala 4 (donde parte el juego), cerrando el círculo. Si prefieres terminar en la sala de recepción (inicio exacto), se mueve el expediente.
4. **Primera vez vs. regreso al hospital:** el hospital es el mismo mapa; la diferencia la hace el flag `PersecucionFinal` (payaso, luces, mensaje). Se puede sumar un cambio de ambiente (luces rojas, puertas cerradas).

## Pendiente técnico
- Probar el tramo 5 (nav dinámico; el volumen cubre hasta Y -19500).
- Música/ambiente distinto por tramo, subtítulos y el rastreo de páginas 16–40 en la Casona.
