// ============================================
// AUDIO CONFIGURATION
// ============================================
// Centralized audio settings for the AVS engine
// All volumes are linear (0-1 scale)
// ============================================

/**
 * Default volumes for each voice type (linear 0-1)
 * Lower values for comfortable listening
 */
export const DEFAULT_VOLUMES = {
  Martigli: 0.15,
  "Martigli-Binaural": 0.15,
  Binaural: 0.12,
  Symmetry: 0.1, // Can be harsh, start lower
  Noise: 0.08, // Background element
};

/**
 * Master volume default (linear 0-1)
 */
export const DEFAULT_MASTER_VOLUME = 0.3;

/**
 * Get default volume for a voice type
 * Returns preset iniVolume if specified, otherwise uses default
 */
export function getDefaultVolume(voiceType, iniVolume = null) {
  if (iniVolume !== null && iniVolume !== undefined) {
    return iniVolume;
  }
  return DEFAULT_VOLUMES[voiceType] ?? 0.25;
}

