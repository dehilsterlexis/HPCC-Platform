/*##############################################################################

    HPCC SYSTEMS software Copyright (C) 2012 HPCC Systems®.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
############################################################################## */

import AggCommon;
AggCommon.CommonDefinitions();
#option ('optimizeDiskSource',true);
#option ('optimizeChildSource',true);
#option ('optimizeIndexSource',true);
#option ('optimizeThorCounts',false);
#option ('countIndex',false);

//A mistyped query, but interesting never the less - needs to access a stored variable from the child.
secondBookNameX := (string20)sort(sqNamesTable5.books, name)[2].name;
output(sqHousePersonBookDs, { dataset people := table(persons, { secondBookNameX, sumage := sum(group, aage) }, secondBookNameX, few)});
