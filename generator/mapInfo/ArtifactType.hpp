#pragma once

#include "../global/Global.hpp"
#include "../global/Random.hpp"

enum class ArtifactType {

    ARTIFACT_UNKNOWN = 0,

    LOW_TIER_TAB,
    ARTIFACT_LADYBIRD_OF_LUCK,
    ARTIFACT_STATESMANS_MEDAL,
    ARTIFACT_CLOVER_OF_FORTUNE,
    ARTIFACT_EMBLEM_OF_COGNIZANCE,
    ARTIFACT_RING_OF_VITALITY,
    ARTIFACT_TALISMAN_OF_MANA,
    ARTIFACT_SPYGLASS,
    ARTIFACT_STOIC_WATCHMAN,
    ARTIFACT_SPECULUM,
    MID_TIER_TAB,
    ARTIFACT_CHARM_OF_MANA,
    ARTIFACT_RING_OF_LIFE,
    ARTIFACT_BADGE_OF_COURAGE,
    ARTIFACT_SHIELD_OF_THE_DWARVEN_LORDS,
    ARTIFACT_BUCKLER_OF_THE_GNOLL_KING,
    ARTIFACT_GREATER_GNOLLS_FLAIL,
    ARTIFACT_DRAGON_WING_TABARD,
    ARTIFACT_RING_OF_THE_WAYFARER,
    ARTIFACT_RING_OF_INFINITE_GEMS,
    ARTIFACT_EQUESTRIANS_GLOVES,
    ARTIFACT_SANDALS_OF_THE_SAINT,
    ARTIFACT_PENDANT_OF_LIFE,
    ARTIFACT_PENDANT_OF_COURAGE,
    ARTIFACT_PENDANT_OF_FREE_WILL,
    ARTIFACT_PENDANT_OF_HOLINESS,
    ARTIFACT_PENDANT_OF_TOTAL_RECALL,
    ARTIFACT_CAPE_OF_CONJURING,
    HIGH_TIER_TAB,
    ARTIFACT_BOWSTRING_OF_THE_UNICORNS_MANE,
    ARTIFACT_BOOTS_OF_POLARITY,
    ARTIFACT_NECKLACE_OF_SWIFTNESS,
    ARTIFACT_VIAL_OF_LIFEBLOOD,
    ARTIFACT_NECKLACE_OF_OCEAN_GUIDANCE,
    ARTIFACT_BREASTPLATE_OF_PETRIFIED_WOOD,
    ARTIFACT_SURCOAT_OF_COUNTERPOISE,
    ARTIFACT_HELM_OF_THE_ALABASTER_UNICORN,
    ARTIFACT_ARMOR_OF_WONDER,
    ARTIFACT_GLYPH_OF_GALLANTRY,
    ARTIFACT_SENTINELS_SHIELD,
    ARTIFACT_DRAGON_SCALE_SHIELD,
    ARTIFACT_ANGEL_FEATHER_ARROWS,
    END_TIER_TAB,
};

inline ArtifactType getArtifactFromTier(int tier, RNG &rng) {

    int beginOfTier = static_cast<int>(ArtifactType::LOW_TIER_TAB);
    int endOfTier   = static_cast<int>(ArtifactType::MID_TIER_TAB);

    switch (tier) {
    case 1:
        beginOfTier = static_cast<int>(ArtifactType::LOW_TIER_TAB);
        endOfTier   = static_cast<int>(ArtifactType::MID_TIER_TAB);
        break;
    case 2:

        beginOfTier = static_cast<int>(ArtifactType::MID_TIER_TAB);
        endOfTier   = static_cast<int>(ArtifactType::HIGH_TIER_TAB);
        break;
    case 3:
        beginOfTier = static_cast<int>(ArtifactType::HIGH_TIER_TAB);
        endOfTier   = static_cast<int>(ArtifactType::END_TIER_TAB);
        break;
    default:
        throw runtime_error("Invalid tier for artifact.");
    }

    return static_cast<ArtifactType>(rng.nextInt(beginOfTier + 1, endOfTier - 1));
}
