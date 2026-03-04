//
// Created by ibach on 01.03.2026.
// @author: Ibach
//

#ifndef TEIL1_PARKHAUS_SIMULATION_PLANNUNG_STATS_H
#define TEIL1_PARKHAUS_SIMULATION_PLANNUNG_STATS_H

#include "../types.h"

/**
 * @file Stats.h
 * @brief API fuer Tick-Statistiken und Gesamtaggregation.
 *
 * Erwarteter Ablauf pro Tick:
 * 1) stats_tick_begin(...)
 * 2) waehrend des Ticks mit stats_tick_add_* / stats_tick_set_* befuellen
 * 3) stats_tick_finalize(...)
 * 4) stats_tick_commit(...)
 *
 * Jeder commitete Tick wird in einer doppelt verketteten Liste gespeichert
 * (p_tick_head ... p_tick_tail). Die Aggregation in `StatsSummary`
 * erfolgt erst bei Bedarf am Ende der Simulation ueber diese Tick-Historie.
 */

/**
 * Initialisiert den Statistikcontainer fuer die Tick-Historie.
 *
 * @param p_stats         Zielcontainer
 * @param capacity_total  Gesamtkapazitaet des Parkhauses
 */
int stats_init(StatList *p_stats, uint16_t capacity_total);

/**
 * Gibt alle in der Tick-Liste gespeicherten Elemente frei.
 */
int stats_free(StatList *p_stats);

/**
 * Startet einen neuen Tick-Builder (p_current_tick).
 */
int stats_tick_begin(StatList *p_stats, uint32_t tick);

/**
 * Berechnet Tick-interne Kennzahlen (z.B. Durchschnitte).
 */
int stats_tick_finalize(StatList *p_stats);

/**
 * Haengt den finalisierten Tick an die Liste an.
 */
int stats_tick_commit(StatList *p_stats);

/** Setzt Tick-Ende-Kapazitaetswerte. */
int stats_tick_set_capacity(StatList *p_stats, uint16_t taken, uint16_t free);

/** Erhoeht Ankuenfte fuer den laufenden Tick. */
int stats_tick_add_arrivals_generated(StatList *p_stats, uint16_t amount);

/** Erhoeht Queue-Aufnahmen fuer den laufenden Tick. */
int stats_tick_add_enqueued(StatList *p_stats, uint16_t amount);

/** Erhoeht Einfahrten ins Parkhaus fuer den laufenden Tick. */
int stats_tick_add_entered(StatList *p_stats, uint16_t amount);

/** Erhoeht Ausfahrten aus dem Parkhaus fuer den laufenden Tick. */
int stats_tick_add_departed(StatList *p_stats, uint16_t amount);

/** Erhoeht Anzahl Queue-Rejections fuer den laufenden Tick. */
int stats_tick_add_queue_rejections(StatList *p_stats, uint16_t amount);

/** Setzt Queue-Laenge am Tick-Ende. */
int stats_tick_set_queue_length_end(StatList *p_stats, uint8_t queue_length_end);

/**
 * Addiert Wartezeit eines Fahrzeugs, das in diesem Tick eingetreten ist.
 * Wird als Rohsumme + Zaehler gespeichert und in Gesamtwerten gemittelt.
 */
int stats_tick_add_entered_queue_wait(StatList *p_stats, uint32_t wait_ticks);

/**
 * Addiert Parkdauer eines Fahrzeugs, das in diesem Tick ausgefahren ist.
 * Wird als Rohsumme + Zaehler gespeichert und in Gesamtwerten gemittelt.
 */
int stats_tick_add_departed_parking_duration(StatList *p_stats, uint32_t duration_ticks);

/** Setzt Anzahl Bad-Parking-Faelle fuer diesen Tick. */
int stats_tick_set_bad_parking_cases(StatList *p_stats, uint16_t bad_cases);

/** Erhoeht Anzahl Bad-Parking-Faelle fuer diesen Tick. */
int stats_tick_add_bad_parking_cases(StatList *p_stats, uint16_t amount);

/** Setzt, ob der Blocker "voll" in diesem Tick aktiv war (0/1). */
int stats_tick_set_blocker_full_active(StatList *p_stats, uint8_t active);

/** Liefert den zuletzt commiteten Tick (Tail) oder NULL. */
const StatsTick *stats_get_latest_tick(const StatList *p_stats);

/**
 * Berechnet die aggregierte Gesamtstatistik aus allen gespeicherten Ticks.
 * p_summary muss vom Aufrufer bereitgestellt werden.
 */
int stats_build_summary(const StatList *p_stats, StatsSummary *p_summary);

#endif //TEIL1_PARKHAUS_SIMULATION_PLANNUNG_STATS_H