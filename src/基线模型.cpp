#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <functional>
#include <string>
#include <sstream>
using namespace std;

const int INF = 1000000000;

struct Edge {
    int to;
    int cost;
};

struct Ambulance {
    int id;
    int pos;              // 当前所在路口
    int availableTime;    // 下一次可用时间
};

struct Event {
    int id;
    int occurTime;        // 事件发生时间
    int pos;              // 事件位置
    int level;            // 危急等级
    int type;             // 所需专科类型
};

struct Hospital {
    int id;
    int pos;
    int cap;
    int originalCap;
    vector<int> types;
};

struct Result {
    bool success;
    int ambulanceId;
    int hospitalId;

    int occurTime;
    int dispatchTime;
    int finishTime;

    int waitTime;       // 从事件发生到开始派车的等待时间
    int toSceneTime;    // 救护车到现场的行驶时间
    int responseTime;   // 患者实际响应时间 = waitTime + toSceneTime
    int sendTime;       // 送医时间
    int totalTravel;    // 救护车总行驶时间 = toSceneTime + sendTime
};

struct TimelineLog {
    int time;
    int order;
    string text;
};

vector<vector<Edge> > graph;

string getLevelName(int level) {
    if (level == 1) return "普通";
    if (level == 2) return "较急";
    if (level == 3) return "高危";
    return "未知";
}

string getTypeName(int type) {
    if (type == 1) return "综合急诊";
    if (type == 2) return "创伤外科";
    if (type == 3) return "心血管急救";
    if (type == 4) return "神经急救";
    if (type == 5) return "儿科急救";
    return "未知专科";
}

bool supportType(const Hospital& h, int type) {
    for (int i = 0; i < (int)h.types.size(); i++) {
        if (h.types[i] == type) {
            return true;
        }
    }
    return false;
}

void printHospitalTypes(const Hospital& h) {
    for (int i = 0; i < (int)h.types.size(); i++) {
        cout << getTypeName(h.types[i]);
        if (i != (int)h.types.size() - 1) {
            cout << "、";
        }
    }
}

bool cmpLog(const TimelineLog& a, const TimelineLog& b) {
    if (a.time != b.time) {
        return a.time < b.time;
    }
    return a.order < b.order;
}

void addLog(vector<TimelineLog>& logs, int time, int& order, const string& text) {
    TimelineLog log;
    log.time = time;
    log.order = order;
    log.text = text;
    logs.push_back(log);
    order++;
}

vector<int> dijkstra(int start, int n) {
    vector<int> dist(n + 1, INF);

    priority_queue<
        pair<int, int>,
        vector<pair<int, int> >,
        greater<pair<int, int> >
    > q;

    dist[start] = 0;
    q.push(make_pair(0, start));

    while (!q.empty()) {
        int d = q.top().first;
        int u = q.top().second;
        q.pop();

        if (d != dist[u]) {
            continue;
        }

        for (int i = 0; i < (int)graph[u].size(); i++) {
            int v = graph[u][i].to;
            int w = graph[u][i].cost;

            if (dist[v] > dist[u] + w) {
                dist[v] = dist[u] + w;
                q.push(make_pair(dist[v], v));
            }
        }
    }

    return dist;
}

// 选择当前时刻已经发生、尚未处理的最高优先级事件
int selectNextEvent(const vector<Event>& events,
                    const vector<int>& status,
                    int currentTime) {
    int best = -1;

    for (int i = 1; i < (int)events.size(); i++) {
        if (status[i] != 0) {
            continue;
        }

        if (events[i].occurTime > currentTime) {
            continue;
        }

        if (best == -1) {
            best = i;
        } else {
            if (events[i].level > events[best].level) {
                best = i;
            } else if (events[i].level == events[best].level) {
                if (events[i].occurTime < events[best].occurTime) {
                    best = i;
                } else if (events[i].occurTime == events[best].occurTime &&
                           events[i].id < events[best].id) {
                    best = i;
                }
            }
        }
    }

    return best;
}

// 判断当前是否存在空闲救护车
bool hasAvailableAmbulance(const vector<Ambulance>& ambulances,
                           int currentTime) {
    for (int i = 1; i < (int)ambulances.size(); i++) {
        if (ambulances[i].availableTime <= currentTime) {
            return true;
        }
    }
    return false;
}

// 计算下一个需要推进到的时间
int getNextTime(const vector<Event>& events,
                const vector<int>& status,
                const vector<Ambulance>& ambulances,
                int currentTime) {
    int nextTime = INF;

    for (int i = 1; i < (int)events.size(); i++) {
        if (status[i] == 0 && events[i].occurTime > currentTime) {
            if (events[i].occurTime < nextTime) {
                nextTime = events[i].occurTime;
            }
        }
    }

    for (int i = 1; i < (int)ambulances.size(); i++) {
        if (ambulances[i].availableTime > currentTime) {
            if (ambulances[i].availableTime < nextTime) {
                nextTime = ambulances[i].availableTime;
            }
        }
    }

    return nextTime;
}

int main() {
    cout << "======================================" << endl;
    cout << " 城市急救车辆调度与医院接收匹配系统" << endl;
    cout << " 基线模型：Dijkstra + 在线最近车辆 + 最近医院" << endl;
    cout << "======================================" << endl;
    cout << endl;

    int n, m;
    cout << "请输入路口数量 n 和道路数量 m：";
    cin >> n >> m;

    graph.assign(n + 1, vector<Edge>());

    cout << endl;
    cout << "请输入每条道路信息：" << endl;
    cout << "格式：起点 终点 基础通行时间 拥堵系数" << endl;
    cout << "说明：实际通行时间 = 基础通行时间 × 拥堵系数" << endl;

    for (int i = 0; i < m; i++) {
        int u, v, w, c;
        cout << "第 " << i + 1 << " 条道路：";
        cin >> u >> v >> w >> c;

        int realCost = w * c;

        Edge e1;
        e1.to = v;
        e1.cost = realCost;
        graph[u].push_back(e1);

        Edge e2;
        e2.to = u;
        e2.cost = realCost;
        graph[v].push_back(e2);
    }

    int a;
    cout << endl;
    cout << "请输入救护车数量：";
    cin >> a;

    vector<Ambulance> ambulances(a + 1);

    cout << "请输入每辆救护车初始所在路口：" << endl;
    for (int i = 1; i <= a; i++) {
        int pos;
        cout << "救护车 A" << i << " 初始所在路口：";
        cin >> pos;

        ambulances[i].id = i;
        ambulances[i].pos = pos;
        ambulances[i].availableTime = 0;
    }

    int e;
    cout << endl;
    cout << "请输入急救事件数量：";
    cin >> e;

    vector<Event> events(e + 1);

    cout << "请输入每个急救事件信息：" << endl;
    cout << "格式：发生时间 事件位置 危急等级 所需专科类型" << endl;
    cout << "危急等级：1=普通，2=较急，3=高危" << endl;
    cout << "专科类型：1=综合急诊，2=创伤外科，3=心血管急救，4=神经急救，5=儿科急救" << endl;

    for (int i = 1; i <= e; i++) {
        int occurTime, pos, level, type;
        cout << "事件 E" << i << "：";
        cin >> occurTime >> pos >> level >> type;

        events[i].id = i;
        events[i].occurTime = occurTime;
        events[i].pos = pos;
        events[i].level = level;
        events[i].type = type;
    }

    int h;
    cout << endl;
    cout << "请输入医院数量：";
    cin >> h;

    vector<Hospital> hospitals(h + 1);

    cout << "请输入每所医院信息：" << endl;
    cout << "格式：医院位置 接收容量 支持专科数量 专科1 专科2 ..." << endl;

    for (int i = 1; i <= h; i++) {
        int pos, cap, k;
        cout << "医院 H" << i << "：";
        cin >> pos >> cap >> k;

        hospitals[i].id = i;
        hospitals[i].pos = pos;
        hospitals[i].cap = cap;
        hospitals[i].originalCap = cap;

        for (int j = 0; j < k; j++) {
            int type;
            cin >> type;
            hospitals[i].types.push_back(type);
        }
    }

    bool showProcess = true;
    if (e > 30) {
        showProcess = false;
    }

    cout << endl;
    cout << "======================================" << endl;
    cout << " 输入数据概览" << endl;
    cout << "======================================" << endl;

    cout << "路口数量：" << n << "，道路数量：" << m << endl;
    cout << "救护车数量：" << a << endl;
    cout << "急救事件数量：" << e << endl;
    cout << "医院数量：" << h << endl;

    if (showProcess) {
        cout << endl;
        cout << "救护车信息：" << endl;
        for (int i = 1; i <= a; i++) {
            cout << "救护车 A" << i
                 << " 初始位于路口 " << ambulances[i].pos
                 << "，初始可用时间：0" << endl;
        }

        cout << endl;
        cout << "急救事件信息：" << endl;
        for (int i = 1; i <= e; i++) {
            cout << "事件 E" << i
                 << "：发生时间=" << events[i].occurTime
                 << "，位置=路口" << events[i].pos
                 << "，危急等级=" << getLevelName(events[i].level)
                 << "，所需专科=" << getTypeName(events[i].type)
                 << endl;
        }

        cout << endl;
        cout << "医院信息：" << endl;
        for (int i = 1; i <= h; i++) {
            cout << "医院 H" << i
                 << "：位置=路口" << hospitals[i].pos
                 << "，容量=" << hospitals[i].cap
                 << "，支持专科=";
            printHospitalTypes(hospitals[i]);
            cout << endl;
        }
    } else {
        cout << "当前为中大规模数据，不逐条输出输入明细。" << endl;
    }

    vector<Result> results(e + 1);
    vector<int> status(e + 1, 0);
    // status = 0 表示未处理，1 表示成功，2 表示失败

    for (int i = 1; i <= e; i++) {
        results[i].success = false;
        results[i].ambulanceId = -1;
        results[i].hospitalId = -1;
        results[i].occurTime = events[i].occurTime;
        results[i].dispatchTime = -1;
        results[i].finishTime = -1;
        results[i].waitTime = 0;
        results[i].toSceneTime = 0;
        results[i].responseTime = 0;
        results[i].sendTime = 0;
        results[i].totalTravel = 0;
    }

    int currentTime = 0;
    if (e > 0) {
        currentTime = events[1].occurTime;
        for (int i = 2; i <= e; i++) {
            if (events[i].occurTime < currentTime) {
                currentTime = events[i].occurTime;
            }
        }
    }

    int finishedCount = 0;
    int successCount = 0;

    int totalResponse = 0;
    int totalHighResponse = 0;
    int totalTravel = 0;
    int highSuccessCount = 0;

    vector<TimelineLog> logs;
    int logOrder = 0;

    cout << endl;
    cout << "======================================" << endl;
    cout << " 开始执行在线多轮基线调度" << endl;
    cout << " 调度规则：事件在线到达，高危优先，选择最近空闲救护车和最近可接收医院" << endl;
    cout << "======================================" << endl;

    while (finishedCount < e) {
        int eventIndex = selectNextEvent(events, status, currentTime);

        if (eventIndex == -1 || !hasAvailableAmbulance(ambulances, currentTime)) {
            int nextTime = getNextTime(events, status, ambulances, currentTime);

            if (nextTime == INF) {
                break;
            }

            if (showProcess) {
                stringstream ss;
                ss << "时间点 " << currentTime
                   << "：暂无可调度事件或无空闲救护车，系统时间推进到 "
                   << nextTime;
                addLog(logs, currentTime, logOrder, ss.str());
            }

            currentTime = nextTime;
            continue;
        }

        Event ev = events[eventIndex];

        vector<int> dist = dijkstra(ev.pos, n);

        int bestAmbulance = -1;
        int bestToScene = INF;

        for (int i = 1; i <= a; i++) {
            if (ambulances[i].availableTime > currentTime) {
                continue;
            }

            int toScene = dist[ambulances[i].pos];

            if (toScene >= INF) {
                continue;
            }

            if (bestAmbulance == -1 ||
                toScene < bestToScene ||
                (toScene == bestToScene && i < bestAmbulance)) {
                bestAmbulance = i;
                bestToScene = toScene;
            }
        }

        if (bestAmbulance == -1) {
            int nextTime = getNextTime(events, status, ambulances, currentTime);

            if (nextTime == INF || nextTime == currentTime) {
                status[eventIndex] = 2;
                finishedCount++;
                results[eventIndex].success = false;

                if (showProcess) {
                    stringstream ss;
                    ss << "时间点 " << currentTime
                       << "：事件 E" << eventIndex
                       << " 调度失败，原因：没有救护车能够到达事件现场";
                    addLog(logs, currentTime, logOrder, ss.str());
                }
            } else {
                if (showProcess) {
                    stringstream ss;
                    ss << "时间点 " << currentTime
                       << "：当前空闲救护车无法到达事件 E" << eventIndex
                       << "，系统时间推进到 " << nextTime;
                    addLog(logs, currentTime, logOrder, ss.str());
                }

                currentTime = nextTime;
            }

            continue;
        }

        int bestHospital = -1;
        int bestSend = INF;

        for (int i = 1; i <= h; i++) {
            if (hospitals[i].cap <= 0) {
                continue;
            }

            if (!supportType(hospitals[i], ev.type)) {
                continue;
            }

            int send = dist[hospitals[i].pos];

            if (send >= INF) {
                continue;
            }

            if (bestHospital == -1 ||
                send < bestSend ||
                (send == bestSend && i < bestHospital)) {
                bestHospital = i;
                bestSend = send;
            }
        }

        if (bestHospital == -1) {
            status[eventIndex] = 2;
            finishedCount++;
            results[eventIndex].success = false;

            if (showProcess) {
                stringstream ss;
                ss << "时间点 " << currentTime
                   << "：事件 E" << eventIndex
                   << " 调度失败，原因：没有满足专科能力和容量要求的医院";
                addLog(logs, currentTime, logOrder, ss.str());
            }

            continue;
        }

        int waitTime = currentTime - ev.occurTime;
        int responseTime = waitTime + bestToScene;
        int totalTravelThisTask = bestToScene + bestSend;
        int finishTime = currentTime + totalTravelThisTask;

        if (showProcess) {
            stringstream ss1;
            ss1 << "时间点 " << currentTime
                << "：事件 E" << ev.id
                << " 发生于时间 " << ev.occurTime
                << "，位置路口 " << ev.pos
                << "，危急等级 " << getLevelName(ev.level)
                << "；派出救护车 A" << bestAmbulance
                << "，送往医院 H" << bestHospital
                << "，响应时间 " << responseTime
                << "，送医时间 " << bestSend
                << "，预计完成时间 " << finishTime;
            addLog(logs, currentTime, logOrder, ss1.str());

            stringstream ss2;
            ss2 << "时间点 " << finishTime
                << "：救护车 A" << bestAmbulance
                << " 完成事件 E" << ev.id
                << "，当前位置更新为医院 H" << bestHospital
                << " 所在路口 " << hospitals[bestHospital].pos
                << "，下一次可用时间为 " << finishTime;
            addLog(logs, finishTime, logOrder, ss2.str());
        }

        ambulances[bestAmbulance].pos = hospitals[bestHospital].pos;
        ambulances[bestAmbulance].availableTime = finishTime;
        hospitals[bestHospital].cap--;

        status[eventIndex] = 1;
        finishedCount++;
        successCount++;

        results[eventIndex].success = true;
        results[eventIndex].ambulanceId = bestAmbulance;
        results[eventIndex].hospitalId = bestHospital;
        results[eventIndex].occurTime = ev.occurTime;
        results[eventIndex].dispatchTime = currentTime;
        results[eventIndex].finishTime = finishTime;
        results[eventIndex].waitTime = waitTime;
        results[eventIndex].toSceneTime = bestToScene;
        results[eventIndex].responseTime = responseTime;
        results[eventIndex].sendTime = bestSend;
        results[eventIndex].totalTravel = totalTravelThisTask;

        totalResponse += responseTime;
        totalTravel += totalTravelThisTask;

        if (ev.level == 3) {
            totalHighResponse += responseTime;
            highSuccessCount++;
        }
    }

    for (int i = 1; i <= e; i++) {
        if (status[i] == 0) {
            status[i] = 2;
            results[i].success = false;
            results[i].occurTime = events[i].occurTime;

            if (showProcess) {
                stringstream ss;
                ss << "事件 E" << i
                   << " 最终未能完成调度，标记为失败";
                addLog(logs, currentTime, logOrder, ss.str());
            }
        }
    }

    if (showProcess) {
        sort(logs.begin(), logs.end(), cmpLog);

        cout << endl;
        cout << "======================================" << endl;
        cout << " 在线多轮基线算法调度时间线" << endl;
        cout << "======================================" << endl;

        for (int i = 0; i < (int)logs.size(); i++) {
            cout << logs[i].text << endl;
        }
    } else {
        cout << endl;
        cout << "当前为中大规模数据，不输出详细调度时间线。" << endl;
    }

    double avgResponse = 0.0;
    double highResponse = 0.0;
    double failRate = 0.0;

    if (successCount > 0) {
        avgResponse = (double)totalResponse / successCount;
    }

    if (highSuccessCount > 0) {
        highResponse = (double)totalHighResponse / highSuccessCount;
    }

    if (e > 0) {
        failRate = (double)(e - successCount) * 100.0 / e;
    }

    cout << endl;
    cout << "======================================" << endl;
    cout << " 在线多轮基线算法最终调度结果" << endl;
    cout << "======================================" << endl;

    if (e <= 30) {
        for (int i = 1; i <= e; i++) {
            if (!results[i].success) {
                cout << "事件 E" << i << "：调度失败" << endl;
            } else {
                cout << "事件 E" << i
                     << "：发生时间 " << results[i].occurTime
                     << "，派车时间 " << results[i].dispatchTime
                     << "，救护车 A" << results[i].ambulanceId
                     << "，接诊医院 H" << results[i].hospitalId
                     << "，等待时间 " << results[i].waitTime
                     << "，到现场时间 " << results[i].toSceneTime
                     << "，实际响应时间 " << results[i].responseTime
                     << "，送医时间 " << results[i].sendTime
                     << "，完成时间 " << results[i].finishTime
                     << endl;
            }
        }
    } else {
        cout << "当前为中大规模数据，不逐条输出事件调度结果。" << endl;
    }

    cout << endl;
    cout << "======================================" << endl;
    cout << " 在线多轮基线算法统计结果" << endl;
    cout << "======================================" << endl;

    cout << fixed << setprecision(2);
    cout << "成功调度事件数：" << successCount << " / " << e << endl;
    cout << "任务失败率：" << failRate << "%" << endl;
    cout << "平均实际响应时间：" << avgResponse << endl;
    cout << "高危事件平均实际响应时间：" << highResponse << endl;
    cout << "救护车总行驶时间：" << totalTravel << endl;

    cout << endl;
    cout << "程序运行结束。" << endl;

    return 0;
}

