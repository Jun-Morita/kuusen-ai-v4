# Copyright (c) 2021-2025 Air Systems Research Center, Acquisition, Technology & Logistics Agency(ATLA)
#ルールベースの初期行動判断モデルのようにobservationやactionを必要としないタイプのモデルの登録方法例
import os,json
import ASRCAISim1
from ASRCAISim1.policy import StandalonePolicy

#①Agentクラスオブジェクトを返す関数を定義
def getUserAgentClass(args={}):
    from BasicAirToAirCombatModels01 import BasicAACRuleBasedAgent01
    return BasicAACRuleBasedAgent01

#②Agentモデル登録用にmodelConfigを表すjsonを返す関数を定義
"""
なお、modelConfigとは、Agentクラスのコンストラクタに与えられる二つのjson(dict)のうちの一つであり、設定ファイルにおいて
{
    "Factory":{
        "Agent":{
            "modelName":{
                "class":"className",
                "config":{...}
            }
        }
    }
}の"config"の部分に記載される{...}のdictが該当する。
"""    
def getUserAgentModelConfig(args={}):
    configs=json.load(open(os.path.join(os.path.dirname(__file__),"config.json"),"r"))
    modelType="Fixed"
    if(args is not None):
        modelType=args.get("type",modelType)
    return configs.get(modelType,"Fixed")

#③Agentの種類(一つのAgentインスタンスで1機を操作するのか、陣営全体を操作するのか)を返す関数を定義
"""AgentがAssetとの紐付けに使用するportの名称は本来任意であるが、
　簡単のために1機を操作する場合は"0"、陣営全体を操作する場合は"0"〜"機数-1"で固定とする。
"""
def isUserAgentSingleAsset(args={}):
    #1機だけならばTrue,陣営全体ならばFalseを返すこと。
    return True

#④StandalonePolicyを返す関数を定義
class DummyPolicy(StandalonePolicy):
    """actionを全く参照しない場合、適当にサンプルしても良いし、Noneを与えても良い。
    """
    def step(self,observation,reward,done,info,agentFullName,observation_space,action_space):
        return None #action_space.sample()

def getUserPolicy(args={}):
    return DummyPolicy()

#⑤Blue側だった場合の初期条件を返す関数を定義
def getBlueInitialState(args={}):
    #(1)任意の値を指定
    state=[
        {"pos":[-5000.0,20000.0,-10000.0], "vel":270.0, "heading":270.0,},
        {"pos":[5000.0,20000.0,-10000.0], "vel":270.0, "heading":270.0,},
    ]
    '''
    #(2)一定領域内ランダムで指定
    state=[{
        "pos":gym.spaces.Box(low=np.array([-10000.0,20000.0,-12000.0]),high=np.array([10000.0,25000.0,-6000.0]),dtype=np.float64).sample().tolist(),
        "vel":260.0+20.0*np.random.rand(),
        "heading": 225.0+90.0*np.random.rand(),
    } for i in range(2)]
    '''
    return state
