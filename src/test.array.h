#include <array>
#include <numeric>

#include "Arduino.h"

#include "globals.h"

using std::accumulate;

const std::array test{
    69100UL,
    66795UL,
    65620UL,
    66115UL,
    66501UL,
    66945UL,
    67400UL,
    67790UL,
    68235UL,
    68695UL,
    69131UL,
    69575UL,
    68300UL,
    65651UL,
    63675UL,
    63020UL,
    63365UL,
    63685UL,
    64230UL,
    64530UL,
    65010UL,
    65366UL,
    65875UL,
    66250UL,
    66690UL,
    66995UL,
    64890UL,
    62935UL,
    61466UL,
    61240UL,
    61685UL,
    62055UL,
    62480UL,
    62870UL,
    63276UL,
    63705UL,
    64105UL,
    64515UL,
    64940UL,
    65296UL,
    63805UL,
    61950UL,
    60630UL,
    60310UL,
    60765UL,
    61085UL,
    61511UL,
    61920UL,
    62325UL,
    62700UL,
    63120UL,
    63531UL,
    63920UL,
    64375UL,
    63785UL,
    61560UL,
    59970UL,
    59405UL,
    59390UL,
    59981UL,
    60220UL,
    60760UL,
    61000UL,
    61490UL,
    61830UL,
    62290UL,
    62611UL,
    63090UL,
    63440UL,
    62340UL,
    60405UL,
    58986UL,
    58795UL,
    58995UL,
    59510UL,
    59780UL,
    60285UL,
    60560UL,
    61066UL,
    61340UL,
    61850UL,
    62150UL,
    62615UL,
    62950UL,
    61445UL,
    59676UL,
    58385UL,
    58250UL,
    58560UL,
    58995UL,
    59285UL,
    59830UL,
    60036UL,
    60555UL,
    60865UL,
    61300UL,
    61665UL,
    62086UL,
    62359UL,
    60571UL,
    59030UL,
    57945UL,
    57900UL,
    58235UL,
    58656UL,
    58965UL,
    59475UL,
    59690UL,
    60205UL,
    60540UL,
    60951UL,
    61325UL,
    61730UL,
    62105UL,
    61425UL,
    59440UL,
    58095UL,
    57791UL,
    57810UL,
    58415UL,
    58655UL,
    59080UL,
    59425UL,
    59890UL,
    60165UL,
    60626UL,
    60985UL,
};

auto i = 1U;
auto lastTestRevTime = 0UL;
auto elapsedTime = 0UL;
auto i2 = 0U;

void simulateRotation()
{
    if (micros() - lastTestRevTime < test[i])
    {
        return;
    }
    
    if( i2 == 500 )
    {
        return;
    }

    lastTestRevTime = micros();
    i++;
    auto now = elapsedTime + accumulate(test.begin(), test.begin() + i, 0);

    //Log.infoln("Simuating step %d", i);

    flywheelService.processRotation(now);
    if (i >= std::size(test))
    {
        Log.infoln("Going to sleep. Processed number of rotations: %d", i);
        elapsedTime += accumulate(test.begin(), test.begin() + i, 0);
        i = 0;
        i2++;
    }
}