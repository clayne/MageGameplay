#include "Events.h"

#include "Utility.h"
#include "Settings.h"
#include "Hooks.h"

namespace Events {
    OnHitEventHandler* OnHitEventHandler::GetSingleton() {
        static OnHitEventHandler singleton;
        return std::addressof(singleton);
    }

    RE::BSEventNotifyControl OnHitEventHandler::ProcessEvent(const RE::TESHitEvent* a_event,
                                                             RE::BSTEventSource<RE::TESHitEvent>* a_eventSource) {
        if (!a_event) return RE::BSEventNotifyControl::kContinue;
        
        auto av_to_heal = RE::ActorValue::kMagicka;        
        auto modifier = Settings::regeneration_value * 0.01;        
        
        if (a_event->cause) {
            if (a_event->cause->IsPlayerRef()) {
                if (auto targ = a_event->target.get(); targ->As<RE::Actor>()) {
                    if (const auto player = RE::PlayerCharacter::GetSingleton(); 
                        const auto equipped_right = player->GetEquippedObject(false)) {
                        if ( const auto weapon = equipped_right->As<RE::TESObjectWEAP>();
                            weapon->IsOneHandedSword() || weapon->IsOneHandedAxe() || weapon->IsOneHandedMace()) {
                            if (const auto magicka_pct = Hooks::GetActorValuePercent(player->As<RE::Actor>(), RE::ActorValue::kMagicka);
                                const auto magicka_av =  player->AsActorValueOwner()->GetActorValue(RE::ActorValue::kMagicka)) { 
                                if (magicka_pct <= Settings::trigger_value && player->IsInCombat()) {
                                    player->AsActorValueOwner()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, av_to_heal, player->AsActorValueOwner()->GetBaseActorValue(RE::ActorValue::kMagicka) * modifier);
                                }
                            }
                        }                                                
                    }                    
                }                
            }             
        }
        return RE::BSEventNotifyControl::kContinue;
    }

    void OnHitEventHandler::Register() {
        const auto holder = RE::ScriptEventSourceHolder::GetSingleton();
        holder->AddEventSink(GetSingleton());
        logger::info("Registered hit event handler");
    }
}
