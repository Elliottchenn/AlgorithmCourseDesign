#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <functional>
#include <string>
#include <cmath>
#include <sstream>
using namespace std;

const int INF = 1000000000;

struct Edge {
    int to;
    int cost;             // 实际通行时间
};

struct Ambulance {
    int id;
    int pos;              // 当前路口
    int availableTime;    // 下一次可用时间
};

struct Event {
    int id;
    int occurTime;        // 发生时间
    int pos;              // 事件位置
    int level;            // 1普通 2较急 3高危
    int type;             // 所需专科
};

struct Hospital {
    int id;
    int pos;              // 医院位置
    int cap;              // 当前剩余容量
    int originalCap;      // 初始容量
    vector<int> types;    // 支持专科集合
};

struct Result {
    bool success;
    int ambulanceId;
    int hospitalId;

    int occurTime;
    int dispatchTime;
    int finishTime;

    int waitTime;
    int toSceneTime;
    int responseTime;
    int sendTime;
    int totalTravel;

    double loadPenalty;
    double cost;
};

struct Candidate {//候选方案 
    int ambulanceId;
    int eventId;
    int hospitalId;

    int eventLevel;
    int eventOccurTime;

    int waitTime;
    int toSceneTime;
    int responseTime;
    int sendTime;
    int totalTravel;
    int finishTime;

    double loadPenalty;
    double cost;
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

// 判断医院是否支持某个专科
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

// 时间线日志按时间排序，同一时间按插入顺序排序
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

// Dijkstra 最短路，返回 start 到所有路口的最短通行时间
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

// 医院越满，继续分配到该医院的惩罚越大
double calculateLoadPenalty(const Hospital& h) {
    if (h.originalCap == 0) {
        return 10.0;
    }

    int used = h.originalCap - h.cap;
    return (double)used / h.originalCap * 10.0;
}

// 多目标综合成本：响应时间、送医时间、医院负载、危急等级
double calculateCost(int responseTime, int sendTime, double loadPenalty, int level) {
    return (0.6 * responseTime + 0.3 * sendTime + 0.1 * loadPenalty) / level;
}

// 候选方案排序：先按成本，再按危急等级、发生时间、响应时间和编号
bool cmpCandidate(const Candidate& a, const Candidate& b) {
    const double EPS = 1e-8;

    if (fabs(a.cost - b.cost) > EPS) {
        return a.cost < b.cost;
    }

    if (a.eventLevel != b.eventLevel) {
        return a.eventLevel > b.eventLevel;
    }

    if (a.eventOccurTime != b.eventOccurTime) {
        return a.eventOccurTime < b.eventOccurTime;
    }

    if (a.responseTime != b.responseTime) {
        return a.responseTime < b.responseTime;
    }

    if (a.ambulanceId != b.ambulanceId) {
        return a.ambulanceId < b.ambulanceId;
    }

    if (a.eventId != b.eventId) {
        return a.eventId < b.eventId;
    }

    return a.hospitalId < b.hospitalId;
}

// 当前时间是否有已经发生但未处理的事件
bool hasWaitingEvent(const vector<Event>& events, const vector<int>& status, int currentTime) {
    for (int i = 1; i < (int)events.size(); i++) {
        if (status[i] == 0 && events[i].occurTime <= currentTime) {
            return true;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
        }
    }
    return false;
}

// 当前时间是否有空闲救护车
bool hasAvailableAmbulance(const vector<Ambulance>& ambulances, int currentTime) {
    for (int i = 1; i < (int)ambulances.size(); i++) {
        if (ambulances[i].availableTime <= currentTime) {
            return true;
        }
    }
    return false;
}

// 当前无法调度时，跳到下一个事件发生时间或救护车可用时间
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
                nextTime = ambulances[i].availableTime; // 重要 
            }
        }
    }

    return nextTime;
}

// 判断某事件是否还存在可接收医院
/*医院容量 cap > 0
医院支持事件所需专科
事件到医院路径可达*/ 
bool hasPossibleHospital(const Event& ev,
                         const vector<Hospital>& hospitals,
                         const vector<int>& distFromEvent) {
    for (int i = 1; i < (int)hospitals.size(); i++) {
        if (hospitals[i].cap <= 0) {
            continue;
        }

        if (!supportType(hospitals[i], ev.type)) {
            continue;
        }

        if (distFromEvent[hospitals[i].pos] >= INF) {
            continue;
        }

        return true;
    }

    return false;
}

string makeEventName(int id) {
    stringstream ss;
    ss << "E" << id;
    return ss.str();
}

string makeAmbulanceName(int id) {
    stringstream ss;
    ss << "A" << id;
    return ss.str();
}

string makeHospitalName(int id) {
    stringstream ss;
    ss << "H" << id;
    return ss.str();
}

int main() {
    cout << "======================================" << endl;
    cout << " 城市急救车辆调度与医院接收匹配系统" << endl;
    cout << " 核心算法：Dijkstra + 多目标加权二分图匹配 + 在线多轮调度" << endl;
    cout << "======================================" << endl;

    int n, m;
    cin >> n >> m;

    graph.assign(n + 1, vector<Edge>());

    // 读取道路信息，实际边权 = 基础通行时间 × 拥堵系数
    for (int i = 0; i < m; i++) {
        int u, v, w, c;
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
    cin >> a;

    vector<Ambulance> ambulances(a + 1);

    // 读取救护车初始位置
    for (int i = 1; i <= a; i++) {
        int pos;
        cin >> pos;

        ambulances[i].id = i;
        ambulances[i].pos = pos;
        ambulances[i].availableTime = 0;
    }

    int e;
    cin >> e;

    vector<Event> events(e + 1);

    // 读取急救事件：发生时间、位置、等级、专科
    for (int i = 1; i <= e; i++) {
        int occurTime, pos, level, type;
        cin >> occurTime >> pos >> level >> type;

        events[i].id = i;
        events[i].occurTime = occurTime;
        events[i].pos = pos;
        events[i].level = level;
        events[i].type = type;
    }

    int h;
    cin >> h;

    vector<Hospital> hospitals(h + 1);

    // 读取医院信息：位置、容量、支持专科集合
    for (int i = 1; i <= h; i++) {
        int pos, cap, k;
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

    // 小规模样例展示过程，中大规模样例只输出统计结果
    bool showProcess = true;
    if (e > 30) {
        showProcess = false;
    }

    cout << endl;
    cout << "======================================" << endl;
    cout << " 输入数据概览" << endl;
    cout << "======================================" << endl;
    cout << "路口数量：" << n << endl;
    cout << "道路数量：" << m << endl;
    cout << "救护车数量：" << a << endl;
    cout << "急救事件数量：" << e << endl;
    cout << "医院数量：" << h << endl;

    if (showProcess) {
        cout << endl;
        cout << "救护车初始位置：" << endl;
        for (int i = 1; i <= a; i++) {
            cout << "A" << i << "：路口 " << ambulances[i].pos << endl;
        }

        cout << endl;
        cout << "事件信息：" << endl;
        for (int i = 1; i <= e; i++) {
            cout << "E" << i
                 << "：发生时间 " << events[i].occurTime
                 << "，位置路口 " << events[i].pos
                 << "，等级 " << getLevelName(events[i].level)
                 << "，专科 " << getTypeName(events[i].type)
                 << endl;
        }

        cout << endl;
        cout << "医院信息：" << endl;
        for (int i = 1; i <= h; i++) {
            cout << "H" << i
                 << "：路口 " << hospitals[i].pos
                 << "，容量 " << hospitals[i].cap
                 << "，支持专科 ";
            printHospitalTypes(hospitals[i]);
            cout << endl;
        }
    } else {
        cout << "当前为中大规模数据，不输出逐项明细。" << endl;
    }

    vector<Result> results(e + 1);
    vector<int> status(e + 1, 0);
    // status：0未处理，1成功，2失败

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
        results[i].loadPenalty = 0.0;
        results[i].cost = 0.0;
    }

    // 对每个事件预处理最短路
    vector<vector<int> > distFromEvent(e + 1);

    for (int i = 1; i <= e; i++) {
        distFromEvent[i] = dijkstra(events[i].pos, n);
    }

    // 当前时间从最早事件发生时间开始
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
    cout << " 开始执行在线多轮改进调度" << endl;
    cout << "======================================" << endl;

    while (finishedCount < e) {
        // 没有等待事件或空闲救护车时，直接推进时间
        if (!hasWaitingEvent(events, status, currentTime) ||
            !hasAvailableAmbulance(ambulances, currentTime)) {
            int nextTime = getNextTime(events, status, ambulances, currentTime);

            if (nextTime == INF) {
                break;
            }

            if (showProcess) {
                stringstream ss;
                ss << "[t=" << currentTime << "] "
                   << "暂无可执行匹配，系统时间推进到 t=" << nextTime;
                addLog(logs, currentTime, logOrder, ss.str());
            }

            currentTime = nextTime;
            continue;
        }

        vector<Candidate> candidates;

        // 构造候选方案：空闲救护车 × 已发生未处理事件 × 可接收医院
        for (int ai = 1; ai <= a; ai++) {
            if (ambulances[ai].availableTime > currentTime) {
                continue;
            }

            for (int ei = 1; ei <= e; ei++) {
                if (status[ei] != 0) {
                    continue;
                }

                if (events[ei].occurTime > currentTime) {
                    continue;
                }

                int toScene = distFromEvent[ei][ambulances[ai].pos];

                if (toScene >= INF) {
                    continue;
                }

                int waitTime = currentTime - events[ei].occurTime;
                int responseTime = waitTime + toScene;

                for (int hi = 1; hi <= h; hi++) {
                    if (hospitals[hi].cap <= 0) {
                        continue;
                    }

                    if (!supportType(hospitals[hi], events[ei].type)) {
                        continue;
                    }

                    int sendTime = distFromEvent[ei][hospitals[hi].pos];

                    if (sendTime >= INF) {
                        continue;
                    }

                    double loadPenalty = calculateLoadPenalty(hospitals[hi]);
                    double cost = calculateCost(responseTime,
                                                sendTime,
                                                loadPenalty,
                                                events[ei].level);

                    Candidate cur;
                    cur.ambulanceId = ai;
                    cur.eventId = ei;
                    cur.hospitalId = hi;
                    cur.eventLevel = events[ei].level;
                    cur.eventOccurTime = events[ei].occurTime;
                    cur.waitTime = waitTime;
                    cur.toSceneTime = toScene;
                    cur.responseTime = responseTime;
                    cur.sendTime = sendTime;
                    cur.totalTravel = toScene + sendTime;
                    cur.finishTime = currentTime + cur.totalTravel;
                    cur.loadPenalty = loadPenalty;
                    cur.cost = cost;

                    candidates.push_back(cur);
                }
            }
        }

        // 当前无候选时，检查是否存在因医院限制导致的失败事件
        if (candidates.size() == 0) {
            bool markAnyFail = false;

            for (int i = 1; i <= e; i++) {
                if (status[i] == 0 && events[i].occurTime <= currentTime) {
                    if (!hasPossibleHospital(events[i], hospitals, distFromEvent[i])) {
                        status[i] = 2;
                        finishedCount++;
                        results[i].success = false;
                        markAnyFail = true;

                        if (showProcess) {
                            stringstream ss;
                            ss << "[t=" << currentTime << "] "
                               << "E" << i
                               << " 调度失败：没有满足专科和容量要求的医院";
                            addLog(logs, currentTime, logOrder, ss.str());
                        }
                    }
                }
            }

            if (!markAnyFail) {
                int nextTime = getNextTime(events, status, ambulances, currentTime);

                if (nextTime == INF) {
                    break;
                }

                if (showProcess) {
                    stringstream ss;
                    ss << "[t=" << currentTime << "] "
                       << "当前无可行候选，系统时间推进到 t=" << nextTime;
                    addLog(logs, currentTime, logOrder, ss.str());
                }

                currentTime = nextTime;
            }

            continue;
        }

        // 按综合成本排序后，依次选择不冲突的匹配方案
        sort(candidates.begin(), candidates.end(), cmpCandidate);

        vector<int> ambulanceChosen(a + 1, 0);
        vector<int> eventChosen(e + 1, 0);

        int selectedThisRound = 0;

        for (int i = 0; i < (int)candidates.size(); i++) {
            Candidate cur = candidates[i];

            if (ambulanceChosen[cur.ambulanceId]) {
                continue;
            }

            if (eventChosen[cur.eventId]) {
                continue;
            }

            if (status[cur.eventId] != 0) {
                continue;
            }

            if (ambulances[cur.ambulanceId].availableTime > currentTime) {
                continue;
            }

            if (hospitals[cur.hospitalId].cap <= 0) {
                continue;
            }

            if (!supportType(hospitals[cur.hospitalId], events[cur.eventId].type)) {
                continue;
            }

            ambulanceChosen[cur.ambulanceId] = 1;
            eventChosen[cur.eventId] = 1;

            // 接受匹配后更新救护车状态、医院容量和事件状态
            ambulances[cur.ambulanceId].pos = hospitals[cur.hospitalId].pos;
            ambulances[cur.ambulanceId].availableTime = cur.finishTime;
            hospitals[cur.hospitalId].cap--;

            status[cur.eventId] = 1;
            finishedCount++;
            successCount++;
            selectedThisRound++;

            // 记录该事件的最终调度结果
            results[cur.eventId].success = true;
            results[cur.eventId].ambulanceId = cur.ambulanceId;
            results[cur.eventId].hospitalId = cur.hospitalId;
            results[cur.eventId].occurTime = events[cur.eventId].occurTime;
            results[cur.eventId].dispatchTime = currentTime;
            results[cur.eventId].finishTime = cur.finishTime;
            results[cur.eventId].waitTime = cur.waitTime;
            results[cur.eventId].toSceneTime = cur.toSceneTime;
            results[cur.eventId].responseTime = cur.responseTime;
            results[cur.eventId].sendTime = cur.sendTime;
            results[cur.eventId].totalTravel = cur.totalTravel;
            results[cur.eventId].loadPenalty = cur.loadPenalty;
            results[cur.eventId].cost = cur.cost;

            totalResponse += cur.responseTime;
            totalTravel += cur.totalTravel;

            if (events[cur.eventId].level == 3) {
                totalHighResponse += cur.responseTime;
                highSuccessCount++;
            }

            // 小规模样例记录调度时间线
            if (showProcess) {
                stringstream ss1;
                ss1 << fixed << setprecision(2);
                ss1 << "[t=" << currentTime << "] "
                    << "E" << cur.eventId
                    << "(" << getLevelName(events[cur.eventId].level) << ")"
                    << " -> A" << cur.ambulanceId
                    << " -> H" << cur.hospitalId
                    << " | 等待=" << cur.waitTime
                    << "，到现场=" << cur.toSceneTime
                    << "，响应=" << cur.responseTime
                    << "，送医=" << cur.sendTime
                    << "，完成=" << cur.finishTime
                    << "，cost=" << cur.cost;
                addLog(logs, currentTime, logOrder, ss1.str());

                stringstream ss2;
                ss2 << "[t=" << cur.finishTime << "] "
                    << "A" << cur.ambulanceId
                    << " 完成 E" << cur.eventId
                    << "，当前位置更新为 H" << cur.hospitalId
                    << " 所在路口 " << hospitals[cur.hospitalId].pos;
                addLog(logs, cur.finishTime, logOrder, ss2.str());
            }
        }

        if (selectedThisRound == 0) {
            int nextTime = getNextTime(events, status, ambulances, currentTime);

            if (nextTime == INF) {
                break;
            }

            if (showProcess) {
                stringstream ss;
                ss << "[t=" << currentTime << "] "
                   << "当前轮没有成功匹配，系统时间推进到 t=" << nextTime;
                addLog(logs, currentTime, logOrder, ss.str());
            }

            currentTime = nextTime;
        }
    }

    // 仍未处理的事件统一标记为失败
    for (int i = 1; i <= e; i++) {
        if (status[i] == 0) {
            status[i] = 2;
            results[i].success = false;
            results[i].occurTime = events[i].occurTime;
        }
    }

    if (showProcess) {
        sort(logs.begin(), logs.end(), cmpLog);

        cout << endl;
        cout << "======================================" << endl;
        cout << " 在线多轮改进算法调度时间线" << endl;
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
    cout << " 在线多轮改进算法最终调度结果" << endl;
    cout << "======================================" << endl;

    if (e <= 30) {
        cout << left
             << setw(8) << "事件"
             << setw(8) << "发生"
             << setw(8) << "派车"
             << setw(10) << "救护车"
             << setw(8) << "医院"
             << setw(8) << "等待"
             << setw(8) << "响应"
             << setw(8) << "送医"
             << setw(8) << "完成"
             << setw(10) << "成本"
             << endl;

        for (int i = 1; i <= e; i++) {
            if (!results[i].success) {
                cout << left
                     << setw(8) << makeEventName(i)
                     << setw(8) << "-"
                     << setw(8) << "-"
                     << setw(10) << "-"
                     << setw(8) << "-"
                     << setw(8) << "-"
                     << setw(8) << "-"
                     << setw(8) << "-"
                     << setw(8) << "-"
                     << setw(10) << "失败"
                     << endl;
            } else {
                cout << left
                     << setw(8) << makeEventName(i)
                     << setw(8) << results[i].occurTime
                     << setw(8) << results[i].dispatchTime
                     << setw(10) << makeAmbulanceName(results[i].ambulanceId)
                     << setw(8) << makeHospitalName(results[i].hospitalId)
                     << setw(8) << results[i].waitTime
                     << setw(8) << results[i].responseTime
                     << setw(8) << results[i].sendTime
                     << setw(8) << results[i].finishTime
                     << fixed << setprecision(2)
                     << setw(10) << results[i].cost
                     << endl;
            }
        }
    } else {
        cout << "当前为中大规模数据，不逐条输出事件调度结果。" << endl;
    }

    cout << endl;
    cout << "======================================" << endl;
    cout << " 改进算法统计结果" << endl;
    cout << "======================================" << endl;

    cout << fixed << setprecision(2);
    cout << "成功调度：" << successCount << " / " << e << endl;
    cout << "任务失败率：" << failRate << "%" << endl;
    cout << "平均响应时间：" << avgResponse << endl;
    cout << "高危平均响应时间：" << highResponse << endl;
    cout << "总行驶时间：" << totalTravel << endl;

    cout << endl;
    cout << "程序运行结束。" << endl;

    return 0;
}


