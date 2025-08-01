// Copyright (c) 2021-2025 Air Systems Research Center, Acquisition, Technology & Logistics Agency(ATLA)
#include "R7ContestRewardSample02.h"
#include <algorithm>
#include <ASRCAISim1/Utility.h>
#include <ASRCAISim1/Units.h>
#include <ASRCAISim1/SimulationManager.h>
#include <ASRCAISim1/Asset.h>
#include <ASRCAISim1/Fighter.h>
#include <ASRCAISim1/Agent.h>
#include <ASRCAISim1/Track.h>
#include <BasicAirToAirCombatModels01/BasicAACRuler01.h>

ASRC_PLUGIN_NAMESPACE_BEGIN

using namespace asrc::core;
using namespace util;
using namespace BasicAirToAirCombatModels01;

void R7ContestRewardSample02::initialize(){
    BaseType::initialize();
    pAvoid=getValueFromJsonKRD(modelConfig,"pAvoid",randomGen,+0.0);
    pHitE_head=getValueFromJsonKRD(modelConfig,"pHitE_head",randomGen,-0.0);
    pHitE_tail=getValueFromJsonKRD(modelConfig,"pHitE_tail",randomGen,-0.0);
    pCrash=getValueFromJsonKRD(modelConfig,"pCrash",randomGen,-0.0);
    pHit_head=getValueFromJsonKRD(modelConfig,"pHit_head",randomGen,+0.0);
    pHit_tail=getValueFromJsonKRD(modelConfig,"pHit_tail",randomGen,+0.0);
    pOut=getValueFromJsonKRD(modelConfig,"pOut",randomGen,-0.0);
    pAlive=getValueFromJsonKRD(modelConfig,"pAlive",randomGen,+0.0);
    pFuelShortage=getValueFromJsonKRD(modelConfig,"pFuelShortage",randomGen,-0.0);
}
void R7ContestRewardSample02::serializeInternalState(asrc::core::util::AvailableArchiveTypes & archive, bool full){
    BaseType::serializeInternalState(archive,full);

    if(full){
        ASRC_SERIALIZE_NVP(archive
            ,pAvoid
            ,pHitE_head,pHitE_tail
            ,pCrash
            ,pHit_head,pHit_tail
            ,pOut
            ,pAlive
            ,pFuelShortage
            ,westSider,eastSider
            ,dOut,dLine
            ,forwardAx,sideAx
            ,fuelMargin
            ,distanceFromBase
            ,parents
            ,missiles
            ,assetToTargetAgent
        )

        if(asrc::core::util::isInputArchive(archive)){
            manager->addEventHandler("Crash",[&](const nl::json& args){this->R7ContestRewardSample02::onCrash(args);});//墜落数監視用
            manager->addEventHandler("Hit",[&](const nl::json& args){this->R7ContestRewardSample02::onHit(args);});//撃墜数監視用
        }
    }
    ASRC_SERIALIZE_NVP(archive
        ,checked
        ,deadFighters
    )
}
void R7ContestRewardSample02::onEpisodeBegin(){
    j_target="All";
    this->AgentReward::onEpisodeBegin();
    auto ruler_=getShared<Ruler,Ruler>(manager->getRuler());
    auto o=ruler_->observables;
    westSider=o.at("westSider");
    eastSider=o.at("eastSider");
    dOut=o.at("dOut");
    dLine=o.at("dLine");
    forwardAx=o.at("forwardAx").get<std::map<std::string,Eigen::Vector2d>>();
    sideAx=o.at("sideAx").get<std::map<std::string,Eigen::Vector2d>>();
    fuelMargin=o.at("fuelMargin");
    distanceFromBase=o.at("distanceFromBase").get<std::map<std::string,double>>();
    manager->addEventHandler("Crash",[&](const nl::json& args){this->R7ContestRewardSample02::onCrash(args);});//墜落数監視用
    manager->addEventHandler("Hit",[&](const nl::json& args){this->R7ContestRewardSample02::onHit(args);});//撃墜数監視用
    parents.clear();
    missiles.clear();
    checked.clear();
    deadFighters.clear();
    assetToTargetAgent.clear();
    for(auto&& e:manager->getAssets()){
        if(isinstance<Missile>(e)){
            auto m=getShared<Missile>(e);
            missiles[m->getTeam()].push_back(m);
            checked[m->getFullName()]=false;
        }
    }
    for(auto&& agentFullName:target){
        auto agent=getShared(manager->getAgent(agentFullName));
        for(auto& p:agent->parents){
            auto f=manager->getAsset<Fighter>(p.second->getFullName());
            parents[agentFullName].push_back(f);
            assetToTargetAgent[p.second->getFullName()]=agentFullName;
        }
    }
}
void R7ContestRewardSample02::onCrash(const nl::json& args){
    std::lock_guard<std::mutex> lock(mtx);
    std::shared_ptr<PhysicalAsset> asset=args;
    std::string name=asset->getFullName();
    auto beg=deadFighters.begin();
    auto end=deadFighters.end();
    if(std::find(beg,end,name)==end){
        if(assetToTargetAgent.find(name)!=assetToTargetAgent.end()){
            reward[assetToTargetAgent[name]]+=pCrash;
        }
        deadFighters.push_back(name);
    }
}
void R7ContestRewardSample02::onHit(const nl::json& args){//{"wpn":wpn,"tgt":tgt}
    std::lock_guard<std::mutex> lock(mtx);
    std::shared_ptr<PhysicalAsset> wpn=args.at("wpn");
    std::shared_ptr<PhysicalAsset> tgt=args.at("tgt");
    auto beg=deadFighters.begin();
    auto end=deadFighters.end();
    if(std::find(beg,end,tgt->getFullName())==end){
        auto crs=manager->getRootCRS();//ここは直交座標系の方が適切。
        Eigen::Vector3d vel=tgt->motion.vel(crs).normalized();
        Eigen::Vector3d dir=(wpn->motion.pos(crs)-tgt->motion.pos(crs)).normalized();
        double angle=acos(vel.dot(dir));
        std::string name=tgt->getFullName();
        if(assetToTargetAgent.find(name)!=assetToTargetAgent.end()){
            reward[assetToTargetAgent[name]]+=((pHitE_head+pHitE_tail)+cos(angle)*(pHitE_head-pHitE_tail))/2;
        }
        name=wpn->parent.lock()->getFullName();
        if(assetToTargetAgent.find(name)!=assetToTargetAgent.end()){
            reward[assetToTargetAgent[name]]+=((pHit_head+pHit_tail)+cos(angle)*(pHit_head-pHit_tail))/2;
        }
        deadFighters.push_back(tgt->getFullName());
    }
}
void R7ContestRewardSample02::onInnerStepEnd(){
    auto ruler_=getShared<Ruler,Ruler>(manager->getRuler());
    auto crs=ruler_->getLocalCRS();
    for(auto&& e:manager->getAssets()){
        if(isinstance<Missile>(e)){
            auto m=getShared<Missile>(e);
            if(!checked[m->getFullName()] && m->hasLaunched && !m->isAlive()){
                Track3D tgt=m->target;
                for(auto&& agentFullName:target){
                    for(auto& p:parents[agentFullName]){
                        if(tgt.isSame(p)){
                            reward[agentFullName]+=pAvoid;
                        }
                    }
                }
                checked[m->getFullName()]=true;
            }
        }else if(isinstance<Fighter>(e)){
            auto f=getShared<Fighter>(e);
            if(f->isAlive()){
                std::string team=f->getTeam();
                double outDist=std::max(0.0,abs(sideAx[team].dot(f->pos(crs).block<2,1>(0,0,2,1)))-dOut);
                outDist=std::max(outDist,abs(forwardAx[team].dot(f->pos(crs).block<2,1>(0,0,2,1)))-dLine);
                std::string name=f->getFullName();
                if(assetToTargetAgent.find(name)!=assetToTargetAgent.end()){
                    reward[assetToTargetAgent[name]]+=(outDist/1000.)*pOut*interval[SimPhase::ON_INNERSTEP_END]*manager->getBaseTimeStep();

                    if(f->optCruiseFuelFlowRatePerDistance>0.0){
                        double maxReachableRange=f->fuelRemaining/f->optCruiseFuelFlowRatePerDistance;
                        double distanceFromLine=forwardAx[team].dot(f->pos(crs).block<2,1>(0,0,2,1))+dLine;
	                    double excess=maxReachableRange/(1+fuelMargin)-(distanceFromLine+distanceFromBase[team]);
                        if(excess<0){
                            reward[assetToTargetAgent[name]]+=pFuelShortage*interval[SimPhase::ON_INNERSTEP_END]*manager->getBaseTimeStep();
                        }
                    }
                }
            }
        }
    }
}
void R7ContestRewardSample02::onStepEnd(){
    auto ruler=getShared<BasicAACRuler01>(manager->getRuler());
    if(ruler->endReason!=BasicAACRuler01::EndReason::NOTYET){
        for(auto&& e:manager->getAssets()){
            if(isinstance<Fighter>(e)){
                auto f=getShared<Fighter>(e);
                if(f->isAlive()){
                    std::string name=f->getFullName();
                    if(assetToTargetAgent.find(name)!=assetToTargetAgent.end()){
                        if(ruler->isReturnableToBase(f)){
                            //帰還可能なら生存点
                            reward[assetToTargetAgent[name]]+=pAlive;
                        }else{
                            //帰還不可能なら墜落ペナルティ
                            reward[assetToTargetAgent[name]]+=pCrash;
                        }
                    }
                }
            }
        }
    }
    this->AgentReward::onStepEnd();
}

void exportR7ContestRewardSample02(py::module& m, const std::shared_ptr<asrc::core::FactoryHelper>& factoryHelper)
{
    using namespace pybind11::literals;
    expose_entity_subclass<R7ContestRewardSample02>(m,"R7ContestRewardSample02",py::module_local()) // 投稿時はpy::module_local()を引数に加えてビルドしたものを用いること！
    DEF_FUNC(R7ContestRewardSample02,onCrash)
    DEF_FUNC(R7ContestRewardSample02,onHit)
    DEF_READWRITE(R7ContestRewardSample02,pAvoid)
    DEF_READWRITE(R7ContestRewardSample02,pHitE_head)
    DEF_READWRITE(R7ContestRewardSample02,pHitE_tail)
    DEF_READWRITE(R7ContestRewardSample02,pCrash)
    DEF_READWRITE(R7ContestRewardSample02,pHit_head)
    DEF_READWRITE(R7ContestRewardSample02,pHit_tail)
    DEF_READWRITE(R7ContestRewardSample02,pOut)
    DEF_READWRITE(R7ContestRewardSample02,pAlive)
    DEF_READWRITE(R7ContestRewardSample02,westSider)
    DEF_READWRITE(R7ContestRewardSample02,eastSider)
    DEF_READWRITE(R7ContestRewardSample02,checked)
    DEF_READWRITE(R7ContestRewardSample02,parents)
    DEF_READWRITE(R7ContestRewardSample02,missiles)
    ;
    FACTORY_ADD_CLASS(Reward,R7ContestRewardSample02)
}

ASRC_PLUGIN_NAMESPACE_END
