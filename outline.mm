<map version="0.7.1">
<node ID="_ID_1607972445" TEXT="ClusterGIS">
<node ID="_Freemind_Link_914419653" TEXT="Abstract" POSITION="right"/>
<node ID="Freemind_Link_1043039428" TEXT="Introduction" POSITION="right">
<node ID="Freemind_Link_813445586" TEXT="GIS processing is important"/>
<node ID="Freemind_Link_556787421" TEXT="Desktop GIS"/>
<node TEXT="Database GIS"/>
<node TEXT="Limitations of single processor implimentations">
<font NAME="SansSerif" SIZE="12"/>
</node>
<node TEXT="Parallel DB"/>
<node TEXT="Parallel GRASS"/>
<node TEXT="Problems with desktop programs on clusters"/>
<node ID="Freemind_Link_1618623782" TEXT="Cluster programming model">
<node TEXT="batch"/>
<node TEXT="mpi"/>
<node TEXT="scalable architecture"/>
<node TEXT="spmd"/>
</node>
</node>
<node TEXT="Related Work" POSITION="right"/>
<node ID="_Freemind_Link_1532169814" TEXT="Requirements" POSITION="right">
<node TEXT="Batch mode"/>
<node TEXT="Data distribution options"/>
<node TEXT="Standard geospatial operations"/>
<node TEXT="Scalable/Performance"/>
<node TEXT="Dataset centric"/>
<node TEXT="Easy to use to form new operations">
<node TEXT="initClusterGIS"/>
<node TEXT="loadData (distributed/replicated)"/>
<node TEXT="process (user code here)"/>
<node TEXT="saveData (distributed/replicated)"/>
<node TEXT="finalizeClusterGIS"/>
</node>
</node>
<node ID="_Freemind_Link_1802099137" TEXT="Research" POSITION="right">
<node ID="_Freemind_Link_180227637" TEXT="Operation Set">
<node ID="_Freemind_Link_496223060" TEXT="Data access"/>
<node ID="_Freemind_Link_718276810" TEXT="OGC standard requirements"/>
</node>
<node ID="_Freemind_Link_1090812530" TEXT="Hardware Setup">
<node TEXT="Saguaro, keep to Clovertowns"/>
</node>
<node TEXT="Datasets/Sizes">
<node TEXT="Full employers ~34k rows"/>
<node TEXT="Full parcels ~1.2M rows"/>
</node>
<node ID="_Freemind_Link_1372334616" TEXT="Implementations">
<node ID="_Freemind_Link_1524593929" TEXT="PostGIS"/>
<node ID="_Freemind_Link_85448330" TEXT="Hadoop Prototype"/>
<node ID="_Freemind_Link_1758972818" TEXT="MPI Prototype"/>
</node>
</node>
<node ID="_Freemind_Link_403023813" TEXT="Results" POSITION="right">
<node ID="_Freemind_Link_1686665076" TEXT="Performance Analysis">
<node TEXT="vary procs, maintain data"/>
<node TEXT="vary data, maintain procs"/>
<node TEXT="vary data, vary procs"/>
</node>
<node ID="_Freemind_Link_811247330" TEXT="Comparisons">
<node TEXT="PostGIS to theoretical linear speedup/scaleup"/>
<node TEXT="Theoretical PostGIS to Hadoop"/>
<node TEXT="Theoretical PostGIS to ClusterGIS"/>
<node TEXT="Hadoop to ClusterGIS"/>
</node>
</node>
<node ID="Freemind_Link_1171151043" TEXT="Recomendation" POSITION="right">
<node ID="Freemind_Link_378383597" TEXT="Synthesis"/>
<node ID="Freemind_Link_1868177310" TEXT="Analysis of problem types that are good/bad for this approach"/>
</node>
<node ID="Freemind_Link_1128327514" TEXT="Conclusion" POSITION="right"/>
</node>
</map>
